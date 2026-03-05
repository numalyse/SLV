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
    
    connect(m_toolBar, &SimpleToolbar::removePlayerRequest, this, [this]() {
        emit removePlayerRequest(this);
    });

    connect(m_toolBar, &SimpleToolbar::duplicatePlayerRequested, this, [this](){
        emit duplicatePlayerRequest(this);
    });

    m_mediaWidget = new MediaWidget(this);

    connect(m_toolBar, &Toolbar::playRequest, this, &PlayerWidget::play);
    connect(m_toolBar, &Toolbar::pauseRequest, this, &PlayerWidget::pause);
    connect(m_toolBar, &Toolbar::stopRequest, this, &PlayerWidget::stop);
    connect(m_toolBar, &Toolbar::ejectRequest, this, &PlayerWidget::eject);
    connect(m_toolBar, &Toolbar::enableFullscreenRequest, this, &PlayerWidget::enablePlayerFullscreen);
    connect(m_toolBar, &Toolbar::disableFullscreenRequest, this, &PlayerWidget::disablePlayerFullscreen);
    connect(m_toolBar, &SimpleToolbar::enableMuteRequest, this, &PlayerWidget::mute);
    connect(m_toolBar, &SimpleToolbar::disableMuteRequest, this, &PlayerWidget::unmute);
    connect(m_toolBar, &SimpleToolbar::volumeChanged, m_mediaWidget, &MediaWidget::setVolume);
    connect(m_toolBar, &SimpleToolbar::speedChanged, m_mediaWidget, &MediaWidget::setSpeed);
    connect(m_toolBar, &Toolbar::screenshotRequest, m_mediaWidget, &MediaWidget::takeScreenshot);
    connect(m_toolBar, &SimpleToolbar::setPositionRequested, this, &PlayerWidget::setTime);
    connect(m_toolBar, &SimpleToolbar::enableLoopModeRequest, this, &PlayerWidget::enableLoopMode);

    connect(this, &PlayerWidget::playUiUpdateRequested, m_toolBar, &SimpleToolbar::playUiUpdate);
    connect(this, &PlayerWidget::pauseUiUpdateRequested, m_toolBar, &SimpleToolbar::pauseUiUpdate);
    connect(this, &PlayerWidget::muteUiUpdateRequested, m_toolBar, &SimpleToolbar::muteUiUpdate);
    connect(this, &PlayerWidget::unmuteUiUpdateRequested, m_toolBar, &SimpleToolbar::unmuteUiUpdate);
    connect(this, &PlayerWidget::ejectUiUpdateRequested, m_toolBar, &SimpleToolbar::ejectUiUpdate);
    connect(this, &PlayerWidget::stopUiUpdateRequested, m_toolBar, &SimpleToolbar::stopUiUpdate);
    connect(this, &PlayerWidget::enableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::enableLoopUiUpdate);
    connect(this, &PlayerWidget::disableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::disableLoopUiUpdate);


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
    m_playing = true;
    m_muted = false;
    emit playUiUpdateRequested();
    emit unmuteUiUpdateRequested();
    emit checkPlayersPlayStatusRequested();
    emit checkPlayersMuteStatusRequested();
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

void PlayerWidget::play()
{
    if (m_mediaWidget->play()){
        m_playing = true;
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }
}

void PlayerWidget::pause()
{
    if (m_mediaWidget->pause()){
        m_playing = false;
        emit pauseUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }

}

void PlayerWidget::stop()
{
    if(m_mediaWidget->stop()){
        m_playing = false;
        emit stopUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    };

}

void PlayerWidget::eject()
{
    if(m_mediaWidget->eject()){
        m_playing = false;
        emit ejectUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }

}

void PlayerWidget::mute()
{
    if(m_mediaWidget->mute()){
        m_muted = true;
        emit muteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }

}

void PlayerWidget::unmute()
{
    if(m_mediaWidget->unmute()){
        m_muted = false;
        emit unmuteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }
}

void PlayerWidget::setVolume(const int &vol)
{
    m_mediaWidget->setVolume(vol);
}

void PlayerWidget::setSpeed(const unsigned int &speed)
{
    m_mediaWidget->setSpeed(speed);
}
void PlayerWidget::setTime(int64_t time){
    m_mediaWidget->setTime(time);
}

void PlayerWidget::updateFpsRequest(double newFps){
    m_media_fps = newFps;
    emit updateFpsRequested(newFps);
}

void PlayerWidget::takeScreenshot()
{
    m_mediaWidget->takeScreenshot();
}

void PlayerWidget::enableLoopMode()
{
    m_mediaWidget->enableLoopMode();
    emit enableLoopUiUpdateRequested();
}

void PlayerWidget::disableLoopMode()
{
    m_mediaWidget->disableLoopMode();
    emit disableLoopUiUpdateRequested();
}
