#include "MediaWidget.h"

#include <QFile>
#include <QUrl>
#include <QKeyEvent>
#include <QTimer>

MediaWidget::MediaWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: black");

    // ===== VLC ===== //
    const char* const vlc_args[] = {
        "--quiet",
        "--no-video-title-show",
        "--no-input-fast-seek"
    };

    m_vlc = libvlc_new(3, vlc_args);
    if (!m_vlc) {
        qDebug() << "Erreur création VLC";
        return;
    }

    m_player = libvlc_media_player_new(m_vlc);

    m_eventManager = libvlc_media_player_event_manager(m_player);

    // On lui dit d'écouter le changement de temps, d'appeler notre fonction statique, 
    // et on lui donne 'this' (notre widget) pour qu'il nous le renvoie dans userData
    libvlc_event_attach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);

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
    libvlc_media_player_play(m_player);

}


MediaWidget::~MediaWidget()
{

    // if (m_vlc) {
    //     libvlc_release(m_vlc);
    // }
    if(m_eventManager){
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
    }

    if (m_parseEventManager)
    {
        libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
    }
    

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

void MediaWidget::stop()
{
    if (!m_player) return;

    pause();
    libvlc_media_player_set_position(m_player, 0.0);
    libvlc_media_player_next_frame(m_player);

}

void MediaWidget::eject(){
    if (!m_player || !libvlc_media_player_get_media(m_player)) return;
    libvlc_media_player_release(m_player);
    m_player = libvlc_media_player_new(m_vlc);
}

void MediaWidget::setTime(int64_t time)
{
    if(!m_player) return;
    libvlc_media_player_set_time(m_player, time);
}

void MediaWidget::onVlcEvent(const libvlc_event_t *event, void *userData)
{
    MediaWidget* mediaWidget = reinterpret_cast<MediaWidget*>(userData);

    if (event->type == libvlc_MediaPlayerTimeChanged)
    {
        emit mediaWidget->updateSliderValueRequested(event->u.media_player_time_changed.new_time);

    }else if(event->type == libvlc_MediaParsedChanged ){

        int parseStatus = event->u.media_parsed_changed.new_status;

        if (parseStatus == 4){

            libvlc_media_t* parsedMedia = static_cast<libvlc_media_t*>(event->p_obj);

            if (parsedMedia) {
                libvlc_time_t duration = libvlc_media_get_duration(parsedMedia);
                emit mediaWidget->updateSliderRangeRequested(duration);
                
                // récup métadonnées
                //libvlc_meta_t meta;
                //libvlc_media_get_meta(parsedMedia, meta);


                libvlc_media_track_t** tracks = nullptr;
                unsigned int tracksCount = libvlc_media_tracks_get(parsedMedia, &tracks);
                float mediaFps = 0.0f;

                for (unsigned int ITrack = 0; ITrack < tracksCount; ++ITrack) 
                {
                    
                    if (tracks[ITrack]->i_type == libvlc_track_video) 
                    {
                        unsigned int num = tracks[ITrack]->video->i_frame_rate_num;
                        unsigned int den = tracks[ITrack]->video->i_frame_rate_den;
                        
                        if (den > 0) {
                            mediaFps = static_cast<float>(num) / static_cast<float>(den);
                        }
                    
                        break; 
                    }
                }

                if (tracksCount > 0) {
                    libvlc_media_tracks_release(tracks, tracksCount);
                }

                if (mediaFps > 0.0f) {
                    emit mediaWidget->updateFpsRequested(mediaFps);
                }

            }
        }
        else  {
            qDebug() << "Le parsing a changé de statut, mais n'est pas terminé. Statut :" << parseStatus;
        }

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

void MediaWidget::setMediaFromPath(const QString& filePath)
{
    if (!m_player)
        return;

    QString pathCopy = filePath;

    // La méthode stop de libvlc est bloquante, on utilise un appel asynchrone pour éviter un deadlock.
    QMetaObject::invokeMethod(this, [this, pathCopy]() {

        libvlc_media_player_stop(m_player);

        QUrl url = QUrl::fromLocalFile(pathCopy);
        QByteArray urlBytes =
            url.toString(QUrl::FullyEncoded).toUtf8();

        libvlc_media_t* media =
            libvlc_media_new_location(m_vlc, urlBytes.constData());

        if (!media)
            return;

        if(m_parseEventManager){ 
            libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
            m_parseEventManager = nullptr;
        }   

        m_parseEventManager = libvlc_media_event_manager(media);
        libvlc_event_attach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
        libvlc_media_parse_with_options(media, libvlc_media_parse_local, 0);

        libvlc_media_player_set_media(m_player, media);

        libvlc_media_release(media);

        libvlc_media_player_play(m_player);

    }, Qt::QueuedConnection);
}


