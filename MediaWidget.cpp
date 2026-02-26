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

    // ===== VLC ===== //
    const char* const vlc_args[] = {
        "--quiet",
        "--aout=directsound"
    };

    m_vlc = libvlc_new(2, vlc_args);
    if (!m_vlc) {
        qDebug() << "Erreur création VLC";
        return;
    }

    QString path = "C:/Users/kviguier/Videos/extraitavare.mp4";
    QFile f(path);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(path);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    libvlc_media_t *media =
        libvlc_media_new_location(m_vlc, urlBytes.constData());
    // qDebug() << "Metadata : " << libvlc_media_get_meta(media, libvlc_meta_t::libvlc_meta_Date);


    if (!media) {
        qDebug() << "Erreur chargement média";
        return;
    }

    m_player = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

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
    removeMedia();

    // if (m_vlc) {
    //     libvlc_release(m_vlc);
    // }
}

void MediaWidget::removeMedia()
{
    if (m_player) {
        libvlc_media_player_stop(m_player);
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
    if(!m_player) return;
    libvlc_media_player_stop(m_player);
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
        QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

        libvlc_media_t* media =
            libvlc_media_new_location(m_vlc, urlBytes.constData());

        if (!media)
            return;

        libvlc_media_player_set_media(m_player, media);
        libvlc_media_release(media);

        libvlc_media_player_play(m_player);

    }, Qt::QueuedConnection);
}
