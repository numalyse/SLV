#include "PlayerWidget.h"
#include <QDebug>
#include <QApplication>
#include <QResizeEvent>
#include <QShowEvent>
#include <QFile>
#include <QKeyEvent>
#include <QToolBar>
#include <QVBoxLayout>


PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    //setMinimumSize(640, 360);
    //resize(800, 450);

    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ===== Toolbar ===== //
    m_toolBar = new QToolBar(this);
    m_actionPlayPause = m_toolBar->addAction("▶ / ⏸");
    m_actionStop      = m_toolBar->addAction("⏹");

    connect(m_actionPlayPause, &QAction::triggered,
            this, &PlayerWidget::togglePlayPause);
    connect(m_actionStop, &QAction::triggered,
            this, &PlayerWidget::stop);


    m_videoWidget = new QWidget(this);
    m_videoWidget->setAttribute(Qt::WA_NativeWindow);
    m_videoWidget->setAttribute(Qt::WA_DontCreateNativeAncestors);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_videoWidget);
    layout->addWidget(m_toolBar);

    // ===== VLC ===== //
    m_vlc = libvlc_new(0, nullptr);
    if (!m_vlc) {
        qDebug() << "Erreur création VLC";
        return;
    }

    QString path = "C:/Users/cmauri12/Videos/ici.mp4";
    QFile f(path);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(path);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    libvlc_media_t *media =
        libvlc_media_new_location(m_vlc, urlBytes.constData());

    if (!media) {
        qDebug() << "Erreur chargement média";
        return;
    }

    m_player = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(
        m_player,
        reinterpret_cast<void*>(m_videoWidget->winId()));
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

PlayerWidget::~PlayerWidget()
{
    if (m_player) {
        libvlc_media_player_stop(m_player);
        libvlc_media_player_release(m_player);
    }

    if (m_vlc) {
        libvlc_release(m_vlc);
    }
}

void PlayerWidget::play()
{
    if (!m_player) return;
    libvlc_media_player_play(m_player);
}

void PlayerWidget::pause()
{
    if (!m_player) return;
    libvlc_media_player_set_pause(m_player, 1);
}

void PlayerWidget::togglePlayPause()
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

void PlayerWidget::stop()
{
    if(!m_player) return;
    libvlc_media_player_stop(m_player);
}


// ===== Event ===== //

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    emit activated(this);
    QWidget::mousePressEvent(event);
}

void PlayerWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        togglePlayPause();
    } else {
        QWidget::keyPressEvent(event);
    }
}


void PlayerWidget::setActive(bool active)
{
    setStyleSheet(active
                      ? "border: 2px solid #2a82da;"
                      : "border: none;");
}
