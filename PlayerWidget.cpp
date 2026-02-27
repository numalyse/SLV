#include "PlayerWidget.h"
#include "Toolbars/SimpleToolbar.h"

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
    m_toolBar = new SimpleToolbar(this);
    connect(m_toolBar, &SimpleToolbar::removePlayerRequest, this, [&]() {
        emit removePlayerRequest(this);
    });

    m_mediaWidget = new MediaWidget(this);

    connect(m_toolBar, &Toolbar::playRequested, this, &PlayerWidget::play);
    connect(m_toolBar, &Toolbar::pauseRequested, this, &PlayerWidget::pause);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_mediaWidget);
    layout->addWidget(m_toolBar);

}

// PlayerWidget::~PlayerWidget()
// {
//     if (m_player) {
//         libvlc_media_player_stop(m_player);
//         libvlc_media_player_release(m_player);
//     }

//     if (m_vlc) {
//         libvlc_release(m_vlc);
//     }
// }

void PlayerWidget::removeMedia()
{
    m_mediaWidget->removeMedia();
}

void PlayerWidget::setActive(bool active)
{
    setStyleSheet(active
                      ? "border: 2px solid #2a82da;"
                      : "border: none;");
}

void PlayerWidget::setMediaFromPath(const QString& filePath)
{
    m_mediaWidget->setMediaFromPath(filePath);
}


// slots 

void PlayerWidget::play(){
    m_mediaWidget->play();
    // forcer la toolbar a être off
}

void PlayerWidget::pause(){
    m_mediaWidget->pause();
    //forcer la toolbar a êtr en on
}