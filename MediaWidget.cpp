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
#include <QDebug>


MediaWidget::MediaWidget(QWidget *parent)
    : QWidget{parent}
{

    m_blackFrame = new QFrame(this);
    m_blackFrame->setStyleSheet("background: black;");
    m_blackFrame->lower();
    m_mediaSurface = new QWidget(this);
    m_mediaSurface->setAttribute(Qt::WA_NativeWindow);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // ===== VLC ===== //

    const char* const vlc_args[] = {
        "--quiet",
        "--aout=directsound",
        "--no-video-title-show",
        "--no-input-fast-seek"
    };

    m_vlcInstance = libvlc_new(4, vlc_args);
    if (!m_vlcInstance) {
        qDebug() << "Erreur création VLC";
        return;
    }
    m_player = libvlc_media_player_new(m_vlcInstance);

    createEventManager();

    managePlayerSystem();
    m_eventManager = libvlc_media_player_event_manager(m_player);

    // On lui dit d'écouter le changement de temps, d'appeler notre fonction statique, 
    // et on lui donne 'this' (notre widget) pour qu'il nous le renvoie dans userData
    libvlc_event_attach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
    libvlc_event_attach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);
    connect(this, &MediaWidget::mediaFinished, &SignalManager::instance(), &SignalManager::mediaWidgetMediaFinished);
    connect(&SignalManager::instance(), &SignalManager::extendedToolbarHideImageEnabled, this, &MediaWidget::hideMedia);
    connect(&SignalManager::instance(), &SignalManager::extendedToolbarHideImageDisabled, this, &MediaWidget::showMedia);

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
        reinterpret_cast<void*>(m_mediaSurface->winId()));
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(
        m_player,
        reinterpret_cast<void*>(m_mediaSurface->winId()));
#else
    libvlc_media_player_set_xwindow(
        m_player,
        m_mediaSurface->winId());
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
    emit mediaPlayerEjected();
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
    const QString & volStr = QString::number(vol);
    emit volumeChanged(volStr);
    emit SignalManager::instance().mediaVolumeChanged(volStr);
}

/// @brief Change media player rate
/// @param speedIndex = 0 : x0.25, 1 : x0.5, 2 : x0.75, 3 : x1, 4 : x1.25, 5 : x1.5, 6 : x2
void MediaWidget::setSpeed(const unsigned int &speedIndex)
{
    if (!m_player) return;
    int err = libvlc_media_player_set_rate(m_player, m_speedSteps[speedIndex]);
    if(err != -1){
        const QString & speedStr = QString::number(m_speedSteps[speedIndex]);
        emit speedChanged(speedStr);
        emit SignalManager::instance().mediaSpeedChanged(speedStr);
    }
}

/// @brief Take a screenshot of the current frame
void MediaWidget::takeScreenshot()
{
    if (!m_player) return;
    QString captureDirectory = QDir::homePath() + "/SLV_Content/screenshot.png"; // nommer comme il faut avec le format nom_film_HH_MM_SS_FF.png ou jpg

    // if there is a problem with media resolution here, make sure to recieve Media::resolutionParsed(tuple<int, int>) signal first
    if(m_rotationIndex % 2 == 0)
        libvlc_video_take_snapshot(m_player, 0, captureDirectory.toUtf8(), m_media->width(), m_media->height());
    else
        // libvlc_video_take_snapshot(m_player, 0, captureDirectory.toUtf8(), m_media->width(), m_media->height());
        libvlc_video_take_snapshot(m_player, 0, captureDirectory.toUtf8(), m_media->height(), m_media->width());
}

void MediaWidget::setTime(int64_t time)
{
    if(!m_player) return;
    libvlc_media_player_set_time(m_player, time);
}

void MediaWidget::moveTimeBackward()
{
    int64_t time = -5000;
    int64_t currentTime = libvlc_media_player_get_time(m_player);
    if(currentTime + time > 5000){
        emit SignalManager::instance().timelineSetPosition(currentTime + time);
    }else{
        emit SignalManager::instance().timelineSetPosition(0);
    }
}

void MediaWidget::moveTimeForward()
{
    int64_t time = 5000;
    int64_t currentTime = libvlc_media_player_get_time(m_player);
    emit SignalManager::instance().timelineSetPosition(currentTime + time);
}

void MediaWidget::enableLoopMode()
{
    m_loopActivated = true;
}

void MediaWidget::disableLoopMode()
{
    m_loopActivated = false;
}

void MediaWidget::hideMedia()
{
    qDebug() << "hide Media";
    m_blackFrame->raise();
}

void MediaWidget::showMedia()
{
    m_blackFrame->lower();
}

void MediaWidget::startRecord()
{
    if(!m_player || !m_media) return;
    m_startRecordTime = libvlc_media_player_get_time(m_player);
    if(m_media->type() != MediaType::Image){
        libvlc_media_add_option(m_media->vlcMedia(), (":sout=#duplicate{dst=display,dst=std{access=file,mux=ps,dst=C:/Users/kviguier/Desktop/test.mp4}"));
        libvlc_media_add_option(m_media->vlcMedia(), ":sout-keep");
    }
}

void MediaWidget::endRecord()
{
    if(!m_player) return;
    int endRecordTime = libvlc_media_player_get_time(m_player);
}

void MediaWidget::rotate()
{
    if(!m_player || !m_media) return;
    float pos = libvlc_media_player_get_position(m_player);
    bool wasPlaying = libvlc_media_player_is_playing(m_player) != 0;

    releaseEventManager();

    libvlc_media_player_stop(m_player);
    libvlc_media_player_release(m_player);
    libvlc_release(m_vlcInstance);

    m_rotationIndex++;
    if(m_rotationIndex > 3)
        m_rotationIndex = 0;

    if(m_rotationIndex != 0){
        const char* const vlc_args[] = {
            "--quiet",
            "--aout=directsound",
            "--no-video-title-show",
            "--no-input-fast-seek",
            "--video-filter=transform",
            m_rotationSteps[m_rotationIndex]
        };

        m_vlcInstance = libvlc_new(6, vlc_args);
    }
    else{
        const char* const vlc_args[] = {
            "--quiet",
            "--aout=directsound",
            "--no-video-title-show",
            "--no-input-fast-seek",
        };

        m_vlcInstance = libvlc_new(4, vlc_args);
    }
    m_player = libvlc_media_player_new(m_vlcInstance);

    createEventManager();

    managePlayerSystem();
    createMedia(m_media->filePath());
    libvlc_media_player_set_media(m_player, m_media->vlcMedia());

    libvlc_media_player_play(m_player);
    libvlc_media_player_set_position(m_player, pos);

    // shows a black screen when rotating but playing again shows the media back
    if(!wasPlaying)
        pause();

}

/// @brief Ecoute les évènements vlc, lors du changement du temps envoie un signal.
/// @param event 
/// @param userData 
void MediaWidget::onVlcEvent(const libvlc_event_t *event, void *userData)
{
    MediaWidget* mediaWidget = reinterpret_cast<MediaWidget*>(userData);

    if (event->type == libvlc_MediaPlayerTimeChanged)
    {
        emit mediaWidget->vlcTimeChanged(event->u.media_player_time_changed.new_time);
    }
    else if(event->type == libvlc_MediaPlayerEndReached){

        QMetaObject::invokeMethod(mediaWidget, [mediaWidget]() {
            qDebug() << "Vidéo terminée" << mediaWidget->m_loopActivated;

            // cas où on est pas en mode loop
            if (!mediaWidget->m_loopActivated){
                libvlc_media_player_stop(mediaWidget->m_player);
                mediaWidget->play();
                mediaWidget->pause();
                emit mediaWidget->mediaFinished();
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

void MediaWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_mediaSurface->setGeometry(rect());
    m_blackFrame->setGeometry(rect());
}

/// @brief Stops the current media player and load a new media from a path
/// @param QString filePath : string containing the path of the media
bool MediaWidget::setMediaFromPath(const QString& filePath)
{
    if (!m_player)
        return false;

    QString pathCopy = filePath;

    (filePath);

    createMedia(filePath);

    if(!m_media->vlcMedia()) return false;

    // La méthode stop de libvlc est bloquante, on utilise un appel asynchrone pour éviter un deadlock.
    QMetaObject::invokeMethod(this, [this, pathCopy](){

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

    return true;

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
    m_media = new Media(filePath, this, m_vlcInstance);
    emit nameUiUpdateRequested(m_media->fileName());
    connect(m_media, &Media::fpsParsed, this, &MediaWidget::updateFpsRequested); 
    connect(m_media, &Media::durationParsed, this, &MediaWidget::updateSliderRangeRequested); 
    m_media->parse();
}
