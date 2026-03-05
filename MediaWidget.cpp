#include "MediaWidget.h"

#include "VlcParseHelper.h"
#include "VlcInstance.h"
#include "Media.h"

#include <QFile>
#include <QUrl>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include <QMap>
#include <QPainter>

MediaWidget::MediaWidget(QWidget *parent)
    : QWidget{parent}
{

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // ===== VLC ===== //
    m_player = libvlc_media_player_new(SLV::VlcInstance::get());

    createEventManager();

    managePlayerSystem();

    libvlc_media_player_play(m_player);
}

void MediaWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
}



/// @brief Sets the media player in the application window instead of a new window
void MediaWidget::managePlayerSystem()
{
#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(
        m_player,
        reinterpret_cast<void*>(winId()));
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(
        m_player,
        reinterpret_cast<void*>(m_videoWidget->winId()));
#else
    libvlc_media_player_set_xwindow(
        m_player,
        m_videoWidget->winId());
#endif

}

/// @brief Destructeur qui détache les event managers vlc
MediaWidget::~MediaWidget()
{

    // if (m_vlc) {
    //     libvlc_release(m_vlc);
    // }
    if(m_eventManager){
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);
    }
    
    releaseMedia();

}

bool MediaWidget::play()
{
    if (!m_player || !m_media) return false;
    if (libvlc_media_player_play(m_player) == -1) return false;
    return true;
}

bool MediaWidget::pause()
{
    if (!m_player || !m_media)  return false;
    libvlc_media_player_set_pause(m_player, 1);
    return true;
}

void MediaWidget::togglePlayPause()
{
    if (!m_player || !m_media ) return;

    if (libvlc_media_player_is_playing(m_player)) {
        libvlc_media_player_pause(m_player);
        qDebug() << "Pause";
    } else {
        libvlc_media_player_play(m_player);
        qDebug() << "Play";
    }
}

/// @brief Set the media player position to 0 and pause
bool MediaWidget::stop()
{
    if (!m_player || !m_media ) return false;

    pause();
    libvlc_media_player_set_position(m_player, 0.0);
    libvlc_media_player_next_frame(m_player);
    return true;
}

/// @brief Release the media player and create a new one from MediaWidget instance
bool MediaWidget::eject()
{
    if (!m_player || !libvlc_media_player_get_media(m_player)) return false;

    releaseEventManager();
    releaseMedia();
    libvlc_media_player_release(m_player);
    m_player = libvlc_media_player_new(SLV::VlcInstance::get());
    createEventManager();
    managePlayerSystem();
    return true;
}

/// @brief Mute the media player
bool MediaWidget::mute()
{
    if (!m_player || !m_media ) return false;
    libvlc_audio_set_mute(m_player, 1);
    return true;
}

/// @brief Unmute the media player
bool MediaWidget::unmute()
{
    if (!m_player || !m_media ) return false;
    libvlc_audio_set_mute(m_player, 0);
    return true;
}

/// @brief Change media player volume
/// @param int vol : volume
void MediaWidget::setVolume(const int &vol)
{
    if (!m_player) return;
    libvlc_audio_set_volume(m_player, vol);
}

/// @brief Change media player rate
/// @param speedIndex = 0 : x0.25, 1 : x0.5, 2 : x0.75, 3 : x1, 4 : x1.25, 5 : x1.5, 6 : x2
void MediaWidget::setSpeed(const unsigned int &speedIndex)
{
    if (!m_player) return;
    libvlc_media_player_set_rate(m_player, m_speedSteps[speedIndex]);
}

/// @brief Take a screenshot of the current frame
void MediaWidget::takeScreenshot()
{
    if (!m_player) return;
    QString captureDirectory = QDir::homePath() + "/SLV_Content/screenshot.png"; // nommer comme il faut avec le format nom_film_HH_MM_SS_FF.png ou jpg
    libvlc_video_take_snapshot(m_player, 0, captureDirectory.toUtf8(), 0, 0);
}

void MediaWidget::setTime(int64_t time)
{
    if(!m_player) return;
    libvlc_media_player_set_time(m_player, time);
}

void MediaWidget::enableLoopMode()
{
    m_loopActivated = true;
}

void MediaWidget::disableLoopMode()
{
    m_loopActivated = false;
}

/// @brief Ecoute les évènements vlc, lors du changement du temps envoie un signal.
/// @param event 
/// @param userData 
void MediaWidget::onVlcEvent(const libvlc_event_t *event, void *userData)
{
    MediaWidget* mediaWidget = reinterpret_cast<MediaWidget*>(userData);

    if (event->type == libvlc_MediaPlayerTimeChanged)
    {
        emit mediaWidget->updateSliderValueRequested(event->u.media_player_time_changed.new_time);
    }
    else if(event->type == libvlc_MediaPlayerEndReached){

        QMetaObject::invokeMethod(mediaWidget, [mediaWidget]() {
            qDebug() << "Vidéo terminée" << mediaWidget->m_loopActivated;

            // cas où on est pas en mode loop
            if (!mediaWidget->m_loopActivated){
                libvlc_media_player_stop(mediaWidget->m_player);
                mediaWidget->play();
                mediaWidget->pause();
            }
            // cas où on loop le média
            else{
                libvlc_media_player_stop(mediaWidget->m_player);
                libvlc_media_player_play(mediaWidget->m_player);
            }
        }, Qt::QueuedConnection);

    }
}


// ===== Event ===== //

void MediaWidget::mousePressEvent(QMouseEvent *event)
{
    emit activated(this);
    QWidget::mousePressEvent(event);
}

void MediaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        togglePlayPause();
    } else {
        QWidget::keyPressEvent(event);
    }
}

/// @brief Stops the current media player and load a new media from a path
/// @param QString filePath : string containing the path of the media
void MediaWidget::setMediaFromPath(const QString& filePath)
{
    if (!m_player)
        return;

    QString pathCopy = filePath;

    (filePath);

    createMedia(filePath);

    // La méthode stop de libvlc est bloquante, on utilise un appel asynchrone pour éviter un deadlock.
    QMetaObject::invokeMethod(this, [this, pathCopy]() {

        libvlc_media_player_stop(m_player);

        QUrl url = QUrl::fromLocalFile(pathCopy);
        QByteArray urlBytes =
            url.toString(QUrl::FullyEncoded).toUtf8();

        libvlc_media_t *vlcMedia = m_media->vlcMedia();

        if (!vlcMedia)
            return;

        libvlc_media_player_set_media(m_player, vlcMedia);

        libvlc_media_player_play(m_player);

        emit mediaPlayerLoaded();

    }, Qt::QueuedConnection);
}

/// @brief detach l'event manager avant de release le média, ne fait rien si déjà null
void MediaWidget::releaseMedia(){
    if(m_media){
        delete m_media;
        m_media = nullptr;
    }
}


/// @brief detach les event et free l'event manager
void MediaWidget::releaseEventManager(){
    if(m_eventManager){
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);
        m_eventManager = nullptr;
    }else {
        qDebug() << "MediaWidget : detach event manager alors que le media player est null";
    }
}

/// @brief initialise m_eventManager et attach les events
void MediaWidget::createEventManager(){
    if(m_player){
        m_eventManager = libvlc_media_player_event_manager(m_player);
        libvlc_event_attach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
        libvlc_event_attach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);
    }else {
        qDebug() << "MediaWidget : Create event manager alors que le media player est null";
    }

}


/// @brief Helper pour recréer une classe média et connecter ses signaux
/// @param filePath 
void MediaWidget::createMedia(const QString& filePath){
    releaseMedia();
    m_media = new Media(filePath, this);
    emit nameUiUpdateRequested(m_media->fileName());
    connect(m_media, &Media::fpsParsed, this, &MediaWidget::updateFpsRequested); 
    connect(m_media, &Media::durationParsed, this, &MediaWidget::updateSliderRangeRequested); 
}
