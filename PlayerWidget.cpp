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
    connect(m_toolBar, &Toolbar::stopRequested, m_mediaWidget, &MediaWidget::stop);
    connect(m_toolBar, &Toolbar::ejectRequested, m_mediaWidget, &MediaWidget::eject);
    connect(m_toolBar, &SimpleToolbar::setPositionRequested, this, &PlayerWidget::setTime);
    connect(m_toolBar, &Toolbar::enableFullscreenRequested, this, &PlayerWidget::enablePlayerFullscreen);
    connect(m_toolBar, &Toolbar::disableFullscreenRequested, this, &PlayerWidget::disablePlayerFullscreen);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_mediaWidget);
    layout->addWidget(m_toolBar);

    connect(m_mediaWidget, &MediaWidget::updateSliderRangeRequested, this, &PlayerWidget::updateSliderRangeRequest);
    connect(m_mediaWidget, &MediaWidget::updateSliderValueRequested, this, &PlayerWidget::updateSliderValueRequest);
    connect(m_mediaWidget,  &MediaWidget::updateFpsRequested, this, &PlayerWidget::updateFpsRequest);

    connect(this, &PlayerWidget::updateSliderRangeRequest, m_toolBar, &SimpleToolbar::updateSliderRange);
    connect(this, &PlayerWidget::updateSliderValueRequest, m_toolBar, &SimpleToolbar::updateSliderValue);
    connect(this, &PlayerWidget::updateFpsRequested, m_toolBar, &SimpleToolbar::updateFps);
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

void PlayerWidget::enablePlayerFullscreen()
{
    emit enablePlayerFullscreenRequested(this);
}

void PlayerWidget::disablePlayerFullscreen()
{
    emit disablePlayerFullscreenRequested(this);
}

// slots 

void PlayerWidget::play(){
    m_mediaWidget->play();
    // forcer la toolbar a être off (emit un signal connecté à la toolbar pour mettre à jour l'icone)
}

void PlayerWidget::pause(){
    m_mediaWidget->pause();
    //forcer la toolbar a êtr en on (pareil que play)
}

void PlayerWidget::setTime(int64_t time){
    m_mediaWidget->setTime(time);
}

void PlayerWidget::updateFpsRequest(float newFps){
    m_media_fps = newFps;
    emit updateFpsRequested(newFps);
}

