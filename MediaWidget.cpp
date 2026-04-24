#include "MediaWidget.h"

#include "VlcParseHelper.h"
#include "Media.h"
#include "SequenceExtractionHelper.h"
#include "MediaTransformHelper.h"
#include "PrefManager.h"

#include <QFile>
#include <QUrl>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include <QMap>
#include <QPainter>
#include <QDebug>
#include <QSize>
#include <QRect>
#include <QProcess>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPushButton>
#include <QThreadPool>


// Fonction helper pour appliquer les transformations VLC correctes au média
void applyTransformOptions(libvlc_media_t* vlcMedia, unsigned int rotation, bool hflip, bool vflip) {
    auto transformArgs = getArgsFromTransform(rotation, hflip, vflip);

    // Si size > 4, il y a une transformation à appliquer
    if (transformArgs.size() > 4) {
        // Activer le filtre transform
        libvlc_media_add_option(vlcMedia, "--video-filter=transform");
        qDebug() << "[applyTransformOptions] Filtre transform activé";

        // Si size > 5, il y a une option spécifique de transformation
        if (transformArgs.size() > 5) {
            const char* transformOption = transformArgs.back();
            libvlc_media_add_option(vlcMedia, transformOption);
            qDebug() << "[applyTransformOptions] Option de transformation appliquée:" << transformOption;
        }
    } else {
        qDebug() << "[applyTransformOptions] Aucune transformation à appliquer";
    }
}

MediaWidget::MediaWidget(QWidget *parent)
    : QWidget{parent}
{
    m_mediaSurface = new QWidget(this);
    m_mediaSurface->setAutoFillBackground(false);
    m_mediaSurface->setAttribute(Qt::WA_NativeWindow);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

    setAttribute(Qt::WA_NativeWindow);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

#ifdef Q_OS_MAC
    setenv("VLC_PLUGIN_PATH", "/Applications/VLC.app/Contents/MacOS/plugins", 0);
#endif

    const char* const vlc_args[] = {
        "--quiet",
        "--no-video-title-show",
        "--no-input-fast-seek"
    };

    int argc = sizeof(vlc_args) / sizeof(vlc_args[0]);

#ifdef Q_OS_WIN
    QByteArray pluginPath = QCoreApplication::applicationDirPath()
        .append("/plugins")
        .toLocal8Bit();

    std::string pluginArg = "--plugin-path=" + std::string(pluginPath.constData());

    const char* const vlc_args_win[] = {
        "--quiet",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--aout=directsound",
        pluginArg.c_str(),
        "--verbose=2"
    };
    m_vlcInstance = libvlc_new(4, vlc_args_win);
#else
    m_vlcInstance = libvlc_new(argc, vlc_args);
#endif

    if (!m_vlcInstance) {
        qCritical() << "Erreur création VLC";
        return;
    }

    m_player = libvlc_media_player_new(m_vlcInstance);
    libvlc_video_set_mouse_input(m_player, 0);
    libvlc_video_set_key_input(m_player, 0);

    createEventManager();
    managePlayerSystem();

    m_eventManager = libvlc_media_player_event_manager(m_player);

    connect(this, &MediaWidget::mediaFinished,
            &SignalManager::instance(),
            &SignalManager::mediaWidgetMediaFinished);

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
    if(m_eventManager){
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerEndReached, onVlcEvent, this);
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerPlaying, onVlcEvent, this);
    }

    m_videoCaptureManager.deleteMediaTempDirectory();
    releaseMedia();

    if (m_player) {
        libvlc_media_player_stop(m_player);
        libvlc_media_player_release(m_player);
    }

    if (m_vlcInstance) {
        libvlc_release(m_vlcInstance);
    }
}

bool MediaWidget::play()
{
    if (!m_player || !m_media) return false;
    return libvlc_media_player_play(m_player) != -1;
}

bool MediaWidget::pause()
{
    if (!m_player || !m_media) return false;
    libvlc_media_player_set_pause(m_player, 1);
    return true;
}

void MediaWidget::togglePlayPause()
{
    if (!m_player || !m_media) return;

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
    if (!m_player || !m_media) return false;

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

    m_hflipped = false;
    m_vflipped = false;
    m_rotationIndex = 0;

    QThreadPool::globalInstance()->start([this]() {

        if(m_player){
            libvlc_media_player_stop(m_player);
            libvlc_media_player_release(m_player);
        }

        m_player = libvlc_media_player_new(m_vlcInstance);
        libvlc_video_set_mouse_input(m_player, 0);
        libvlc_video_set_key_input(m_player, 0);

        QMetaObject::invokeMethod(this, [this]() {
            releaseMedia();
            createEventManager();
            managePlayerSystem();
            emit mediaPlayerEjected();
        });
    });

    return true;
}

void MediaWidget::parseTracks()
{
    if (!m_player || !m_media)
        return;

    m_media->parseTracks(m_player);
}

void MediaWidget::setAudioTrack(int trackId)
{
    if (!m_player) return;
    m_currentAudioTrack = trackId;
    libvlc_audio_set_track(m_player, trackId);
    qDebug() << "[MEDIAWIDGET] changement sur : " << trackId;
}

void MediaWidget::setSubtitleTrack(int trackId)
{
    if (!m_player) return;
    m_currentSubtitlesTrack = trackId;
    libvlc_video_set_spu(m_player, trackId);
    qDebug() << "[MEDIAWIDGET] changement sur : " << trackId;
}

QList<QPair<int, QString>> MediaWidget::audioTracks() const
{
    qDebug() << "[MEDIAWIDGET] : audioTracks";
    qDebug() << "[MEDIAWIDGET] media instance =" << m_media;

    if (!m_media || !m_player) return {};

    const auto tracks = m_media->audioTracks();

    for (int i = 0; i < tracks.size(); ++i) {
        qDebug() << "[" << i << "]"
                << "ID:" << tracks[i].first
                << "Name:" << tracks[i].second;
    }

    return tracks;
}

QList<QPair<int, QString>> MediaWidget::subtitlesTracks() const
{
    if (!m_media || !m_player) return {};
    return m_media->subtitlesTracks();
}

void MediaWidget::getAudioTracks() 
{
    emit updateAudioTracksRequested(audioTracks());
    qDebug() << "[MEDIAWIDGET] SEND emit updateAudioTracksRequested";
}

void MediaWidget::getSubtitlesTracks() 
{
    emit updateSubtitlesTracksRequested(m_media->subtitlesTracks());
    qDebug() << "[MEDIAWIDGET] SEND emit updateSubtitlesTracksRequested";
}

void MediaWidget::updateTracks()
{
    qDebug() << "[MEDIAWIDGET] RECEIVED emit tracksParsed";
    getAudioTracks();
    getSubtitlesTracks();
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

    bool cancelScreenShot = false;
    bool isMediaAdjusted = libvlc_video_get_adjust_int(m_player, libvlc_adjust_Enable) == 1 ||
                            libvlc_video_get_adjust_float(m_player, libvlc_adjust_Brightness) != 1 ||
                            libvlc_video_get_adjust_float(m_player, libvlc_adjust_Contrast) != 1 ||
                            libvlc_video_get_adjust_float(m_player, libvlc_adjust_Saturation) != 1 ||
                            libvlc_video_get_adjust_float(m_player, libvlc_adjust_Hue) != 0 ||
                            libvlc_video_get_adjust_float(m_player, libvlc_adjust_Gamma) != 1;

    if(isMediaAdjusted){
        QMessageBox *msg = new QMessageBox(this);
        QPushButton *cancelBtn = new QPushButton(PrefManager::instance().getText("cancel_action"));
        msg->addButton(cancelBtn, QMessageBox::RejectRole);
        msg->setInformativeText(PrefManager::instance().getText("messagebox_screenshot_with_adjustment"));
        msg->setIcon(QMessageBox::Information);
        QPushButton *screenshotWithoutAdjustments = new QPushButton(PrefManager::instance().getText("screenshot_without_adjustment_button"));
        msg->addButton(screenshotWithoutAdjustments, QMessageBox::AcceptRole);
        msg->adjustSize();

        connect(msg, &QMessageBox::accepted, this, [this, &cancelScreenShot](){ libvlc_video_set_adjust_int(m_player, libvlc_adjust_Enable, 0); cancelScreenShot = false;});
        connect(msg, &QMessageBox::rejected, this, [this, &cancelScreenShot](){ cancelScreenShot = true; });

        msg->exec();
        if(cancelScreenShot) return;
    }
    
    auto& prefManager = PrefManager::instance();
    QString capturePath = prefManager.getPref("Paths", "screenshot") + '/' + m_media->fileName() + TimeFormatter::fileFormatMsToHHMMSSFF(getCurrentTime(), m_media->fps()) +".png";

    QByteArray capturePathBytes = capturePath.toUtf8();

    int w = m_media->width();
    int h = m_media->height();

    // if there is a problem with media resolution here, make sure to recieve Media::resolutionParsed(tuple<int, int>) signal first
    if(m_rotationIndex % 2 == 0)
        libvlc_video_take_snapshot(m_player, 0, capturePathBytes.constData(), w, h);
    else
        libvlc_video_take_snapshot(m_player, 0, capturePathBytes.constData(), h, w);

    if(isMediaAdjusted)
         libvlc_video_set_adjust_int(m_player, libvlc_adjust_Enable, 1);
}

void MediaWidget::setTime(int64_t time)
{
    if(!m_player) return;
    m_videoCaptureManager.mediaCutAndConcat(getCurrentTime(), time);
    libvlc_media_player_set_time(m_player, time);
    m_vlcTime = time;
    emit vlcTimeChanged(time);
}

void MediaWidget::moveTimeBackward()
{
    int64_t time = -5000;
    int64_t currentTime = getCurrentTime();
    if(currentTime + time > 5000){
        setTime(currentTime + time);
    }else{
        setTime(0);
    }
}

void MediaWidget::moveTimeForward()
{
    int64_t time = 5000;
    int64_t currentTime = getCurrentTime();
    setTime(currentTime + time);
}

void MediaWidget::enableLoopMode()
{
    m_loopActivated = true;
}

void MediaWidget::disableLoopMode()
{
    m_loopActivated = false;
}

void MediaWidget::enableZoomMode()
{
    m_zoomActivated = true;
    if(!m_player || !m_media) return;
        libvlc_video_set_crop_geometry(m_player, m_zoomHelper.getZoomArg().toUtf8().constData());
}

void MediaWidget::disableZoomMode()
{
    m_zoomActivated = false;
    if(!m_player || !m_media) return;
    libvlc_video_set_crop_geometry(m_player, QString("%1x%2+%3+%4").arg(m_media->width()).arg(m_media->height()).arg(0).arg(0).toUtf8().constData());
}

void MediaWidget::startRecord()
{
    if(!m_player || !m_media) return;
    m_videoCaptureManager.startMediaRecording(getCurrentTime());
}

void MediaWidget::endRecord()
{
    if(!m_player) return;

    int endTime = getCurrentTime();
    pause();
    auto& prefManager = PrefManager::instance();
    QString saveRecordPath = QFileDialog::getSaveFileName(
        this, 
        prefManager.getText("dialog_capture"),
        prefManager.getPref("Paths", "lp_capture")
    );

    if (saveRecordPath.isEmpty()){
        qDebug() << "[MediaWidget] Enregistrement de la capture annulé";
        return;
    }

    QFileInfo fileInfo (saveRecordPath);
    prefManager.setPref("Paths", "lp_capture", fileInfo.absolutePath());

    saveRecordPath += '.' + m_media->fileExtension();
    // SequenceExtractionHelper::extractSequence(m_media->filePath(), m_startRecordTime, libvlc_media_player_get_time(m_player), saveRecordPath);
    m_videoCaptureManager.endMediaRecording(endTime, saveRecordPath);

    m_startRecordTime = -1;
}

void MediaWidget::transformMedia()
{
    if(!m_player || !m_media) return;

    float pos = libvlc_media_player_get_position(m_player);
    bool wasPlaying = libvlc_media_player_is_playing(m_player);

    releaseEventManager();

    libvlc_media_player_stop(m_player);
    libvlc_media_player_release(m_player);

    if (m_vlcInstance) {
        libvlc_release(m_vlcInstance);
    }

#ifdef Q_OS_MAC
    setenv("VLC_PLUGIN_PATH", "/Applications/VLC.app/Contents/MacOS/plugins", 0);
#endif

    auto args = getArgsFromTransform(m_rotationIndex, m_hflipped, m_vflipped);

    m_vlcInstance = libvlc_new(args.size(), args.data());
    m_player = libvlc_media_player_new(m_vlcInstance);
    libvlc_video_set_mouse_input(m_player, 0);
    libvlc_video_set_key_input(m_player, 0);

    createEventManager();

    managePlayerSystem();
    createMedia(m_media->filePath());
    libvlc_media_player_set_media(m_player, m_media->vlcMedia());

    libvlc_media_player_play(m_player);
    libvlc_media_player_set_position(m_player, pos);

    setAudioTrack(m_currentAudioTrack);
    setSubtitleTrack(m_currentSubtitlesTrack);

    // shows a black screen when rotating but playing again shows the media back
    if(!wasPlaying)
        pause();
}

void MediaWidget::rotate()
{
    m_rotationIndex = (m_rotationIndex-1) % 4;
    transformMedia();
}

void MediaWidget::hFlip()
{
    m_hflipped = !m_hflipped;
    transformMedia();
    emit hFlipUiUpdateRequested();
}

void MediaWidget::vFlip()
{
    m_vflipped = !m_vflipped;
    transformMedia();
    vFlipUiUpdateRequested();
}

void MediaWidget::nextFrame()
{
    if(!m_player || !m_media) return;
    pause();

    // libvlc_media_player_next_frame(m_player);
    const int newTime = getCurrentTime() + int(1000/m_media->fps());
    setTime(newTime);
    qDebug() << libvlc_media_player_get_time(m_player) << "," << m_vlcTime;
    emit vlcTimeChanged(m_vlcTime);

}

void MediaWidget::prevFrame()
{
    if(!m_player || !m_media) return;
    pause();
    qDebug() << libvlc_media_player_get_time(m_player) << "," << m_vlcTime;
    const int newTime = getCurrentTime() - int(1000/m_media->fps());
    setTime(newTime);
}

void MediaWidget::adjustMedia(const libvlc_video_adjust_option_t adjustOption, const float value)
{
    if(!m_player || !m_media) return;
    libvlc_video_set_adjust_int(m_player, libvlc_adjust_Enable, 1);
    libvlc_video_set_adjust_float(m_player, adjustOption, value);
    qDebug() << "BRIGHTNESS : " << libvlc_video_get_adjust_float(m_player, libvlc_adjust_Brightness);
    qDebug() << "CONTRAST : " << libvlc_video_get_adjust_float(m_player, libvlc_adjust_Contrast);
    qDebug() << "SATURATION : " << libvlc_video_get_adjust_float(m_player, libvlc_adjust_Saturation);
    qDebug() << "HUE : " << libvlc_video_get_adjust_float(m_player, libvlc_adjust_Hue);
    qDebug() << "GAMMA : " << libvlc_video_get_adjust_float(m_player, libvlc_adjust_Gamma);
}

void MediaWidget::resetAdjustments()
{
    if(!m_player || !m_media) return;
    libvlc_video_set_adjust_int(m_player, libvlc_adjust_Enable, 0);
}

QPoint MediaWidget::getMediaPosRect() const
{
    return m_mediaSurface->mapToGlobal(m_mediaSurface->pos());
}

QRect MediaWidget::getMediaDisplayRect() const
{
    if (m_mediaSize.isEmpty())
        return rect();

    QSize widgetSize = size();

    QSize scaled = m_mediaSize.scaled(widgetSize, Qt::KeepAspectRatio);

    int x = (widgetSize.width() - scaled.width()) / 2;
    int y = (widgetSize.height() - scaled.height()) / 2;

    return QRect(QPoint(x, y), scaled);
}

/// @brief Ecoute les évènements vlc, lors du changement du temps envoie un signal.
/// @param event 
/// @param userData 
void MediaWidget::onVlcEvent(const libvlc_event_t *event, void *userData)
{
    MediaWidget* mediaWidget = reinterpret_cast<MediaWidget*>(userData);
    
    if (!mediaWidget) return;

    if (event->type == libvlc_MediaPlayerTimeChanged)
    {
        mediaWidget->m_vlcTime = event->u.media_player_time_changed.new_time;
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
    else if (event->type == libvlc_MediaPlayerPlaying)
    {
        QMetaObject::invokeMethod(mediaWidget, [mediaWidget]() {

            unsigned width = 0;
            unsigned height = 0;

            libvlc_video_get_size(mediaWidget->m_player, 0, &width, &height);

            if (width > 0 && height > 0)
            {
                mediaWidget->m_mediaSize = QSize(width, height); // Update size
                //emit mediaWidget->mediaSizeChanged(mediaWidget->m_mediaSize);
                //qDebug() << "media size OK:" << width << height;
            }

            if (mediaWidget->m_media->audioTracks().isEmpty() && mediaWidget->m_media->subtitlesTracks().isEmpty()){
                mediaWidget->parseTracks();
                if(!mediaWidget->m_media->audioTracks().isEmpty()){
                    emit mediaWidget->setAudioTrackRequested(mediaWidget->m_currentAudioTrack);
                    emit mediaWidget->setSubtitlesTrackRequested(mediaWidget->m_currentSubtitlesTrack);
                }
            }
            
        }, Qt::QueuedConnection);
    }
}


// ===== Event ===== //

void MediaWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        if (event->modifiers() & Qt::ControlModifier) {
            if(!m_player || !m_media || !m_zoomActivated) return;
            setCursor(Qt::ClosedHandCursor);
            m_isPanning = true;
            m_lastPanPos = event->pos();
        }
        else{
            // emit togglePlayPauseRequested(libvlc_media_player_is_playing(m_player));
        }

        QWidget::mousePressEvent(event);

    }
}

void MediaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_player || !m_media || !m_zoomActivated) return;
    setCursor(Qt::ArrowCursor);
    m_isPanning = false;
}

void MediaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_player || !m_media || !m_zoomActivated) return;
    if(m_isPanning){
        QPoint deltaPos = event->pos() - m_lastPanPos;
        m_zoomHelper.move(deltaPos);
        m_lastPanPos = event->pos();
        libvlc_video_set_crop_geometry(m_player, m_zoomHelper.getZoomArg().toUtf8().constData());
    }
}

void MediaWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    QRect mediaRect = getMediaDisplayRect();
    m_mediaSurface->setGeometry(mediaRect);
    emit mediaRectChanged(mediaRect);
    qDebug() << "mediaWidget size:" << this->size();
    qDebug() << "m_mediaSurface size:" << m_mediaSurface->size();
    qDebug() << "Displayed video rect:" << mediaRect;
    qDebug() << "mediasize:" << m_mediaSize;
}

void MediaWidget::wheelEvent(QWheelEvent *event)
{
    if(!m_player || !m_media || !m_zoomActivated) return;

    int delta = event->angleDelta().y();

    QPointF cursorPosNormalized = QPointF(event->position().x() / geometry().width(), event->position().y() / geometry().height());

    if (delta < 0)
        libvlc_video_set_crop_geometry(m_player, m_zoomHelper.zoom(0.1, cursorPosNormalized).toUtf8().constData());
    else
        libvlc_video_set_crop_geometry(m_player, m_zoomHelper.zoom(-0.1, cursorPosNormalized).toUtf8().constData());
}

/// @brief Stops the current media player and load a new media from a path
/// @param QString filePath : string containing the path of the media
bool MediaWidget::setMediaFromPath(const QString& filePath)
{
    if (!m_player)
        return false;

    QString pathCopy = filePath;

    createMedia(filePath);
    m_videoCaptureManager.setMediaPath(filePath);

    if(!m_media->vlcMedia()) return false;

    // La méthode stop de libvlc est bloquante, on utilise un appel asynchrone pour éviter un deadlock.
    QMetaObject::invokeMethod(this, [this, pathCopy](){

        libvlc_media_player_stop(m_player);

        QUrl url = QUrl::fromLocalFile(pathCopy);
        QByteArray urlBytes =
            url.toString(QUrl::FullyEncoded).toUtf8();

        libvlc_media_t *vlcMedia = libvlc_media_new_location(m_vlcInstance, urlBytes.constData());

        if (!vlcMedia)
            return;

        libvlc_media_player_set_media(m_player, vlcMedia);

        libvlc_media_player_play(m_player);

        emit mediaPlayerLoaded();

    }, Qt::QueuedConnection);

    return true;

}

void MediaWidget::typeParsed(const MediaType type)
{
    if(type == MediaType::Video)
        emit mediaIsVideoParsed();
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
        libvlc_event_detach(m_eventManager, libvlc_MediaPlayerPlaying, onVlcEvent, this);
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
        libvlc_event_attach(m_eventManager, libvlc_MediaPlayerPlaying, onVlcEvent, this);
    }else {
        qDebug() << "MediaWidget : Create event manager alors que le media player est null";
    }

}

/// @brief Helper pour recréer une classe média et connecter ses signaux
/// @param filePath 
void MediaWidget::createMedia(const QString& filePath){
    releaseMedia();
    emit nameUiUpdateRequested(tr(""));
    m_media = new Media(filePath, this, m_vlcInstance);
    QMetaObject::invokeMethod(this, [this]() {
        if (m_media)
            emit nameUiUpdateRequested(m_media->fileName());
    }, Qt::QueuedConnection);
    connect(m_media, &Media::fpsParsed, this, &MediaWidget::updateFpsRequested);
    connect(m_media, &Media::durationParsed, this, &MediaWidget::updateSliderRangeRequested);
    connect(m_media, &Media::resolutionParsed, this, [this](){ m_zoomHelper = ZoomHelper(m_media->width(), m_media->height()); });
    connect(m_media, &Media::tracksParsed, this, &MediaWidget::updateTracks);
    connect(m_media, &Media::typeParsed, this, &MediaWidget::typeParsed);
    m_media->parse();


    m_media->parseTracks(m_player);
}
