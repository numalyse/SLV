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

MediaWidget::MediaWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: black");

    // ===== VLC ===== //
    m_player = libvlc_media_player_new(SLV::VlcInstance::get());

    managePlayerSystem();

    m_eventManager = libvlc_media_player_event_manager(m_player);

    // On lui dit d'écouter le changement de temps, d'appeler notre fonction statique, 
    // et on lui donne 'this' (notre widget) pour qu'il nous le renvoie dans userData
    libvlc_event_attach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
    libvlc_event_attach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);

    libvlc_media_player_play(m_player);
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

    if (m_parseEventManager)
    {
        libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
    }
    
    releaseMedia();

}

void MediaWidget::play()
{
    if (!m_player) return;
    libvlc_media_player_play(m_player);
}

void MediaWidget::pause()
{
    if (!m_player) return;
    libvlc_media_player_set_pause(m_player, 1);
}

void MediaWidget::togglePlayPause()
{
    if (!m_player) return;

    if (libvlc_media_player_is_playing(m_player)) {
        libvlc_media_player_pause(m_player);
        qDebug() << "Pause";
    } else {
        libvlc_media_player_play(m_player);
        qDebug() << "Play";
    }
}

/// @brief Set the media player position to 0 and pause
void MediaWidget::stop()
{
    if (!m_player) return;

    pause();
    libvlc_media_player_set_position(m_player, 0.0);
    libvlc_media_player_next_frame(m_player);

}

/// @brief Release the media player and create a new one from MediaWidget instance
void MediaWidget::eject()
{
    if (!m_player || !libvlc_media_player_get_media(m_player)) return;

    releaseMedia();
    libvlc_media_player_release(m_player);
    m_player = libvlc_media_player_new(SLV::VlcInstance::get());
    managePlayerSystem();
}

/// @brief Mute the media player
void MediaWidget::mute()
{
    if (!m_player) return;
    libvlc_audio_set_mute(m_player, 1);
}

/// @brief Unmute the media player
void MediaWidget::unmute()
{
    if (!m_player) return;
    libvlc_audio_set_mute(m_player, 0);
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

    createMediaMeta(filePath);

    // La méthode stop de libvlc est bloquante, on utilise un appel asynchrone pour éviter un deadlock.
    QMetaObject::invokeMethod(this, [this, pathCopy]() {

        libvlc_media_player_stop(m_player);

        QUrl url = QUrl::fromLocalFile(pathCopy);
        QByteArray urlBytes =
            url.toString(QUrl::FullyEncoded).toUtf8();

        libvlc_media_t  *media = libvlc_media_new_location(SLV::VlcInstance::get(), urlBytes.constData());

        if (!media)
            return;

        libvlc_media_player_set_media(m_player, media);

        libvlc_media_release(media);

        libvlc_media_player_play(m_player);

    }, Qt::QueuedConnection);
}

/// @brief detach l'event manager avant de release le média
void MediaWidget::releaseMedia(){
    if(m_eventManager){
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
        m_eventManager = nullptr;
    }
    if(m_media){
        delete m_media;
        m_media = nullptr;
    }
}

/// @brief Helper pour recréer une classe média et connecter ses signaux
/// @param filePath 
void MediaWidget::createMediaMeta(const QString& filePath){
    if(m_media){
        delete m_media;
    }
    m_media = new Media(filePath, this);
    connect(m_media, &Media::fpsParsed, this, &MediaWidget::updateFpsRequested); 
    connect(m_media, &Media::durationParsed, this, &MediaWidget::updateSliderRangeRequested); 
}
