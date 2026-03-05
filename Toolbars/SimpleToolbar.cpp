#include "Toolbars/SimpleToolbar.h"
#include "TextManager.h"
#include "TimeFormatter.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h" 

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SimpleToolbar.h"

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    m_currentTimeLabel = new QLabel("00:00:00", this);
    m_durationLabel = new QLabel("00:00:00", this);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0,0);
    m_slider->setValue(0);

    m_seekTimer = new QTimer(this);
    m_seekTimer->setSingleShot(true);

    connect(m_slider, &QSlider::sliderPressed, this, [this]() {
        m_draggingSlider = true;
        emit setPositionRequested(m_slider->value());
    });
    connect(m_slider, &QSlider::sliderReleased, this, [this]() {
        m_draggingSlider = false;
    });

    connect(m_slider, &QSlider::sliderMoved, this, [this](){
        m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(m_slider->value(), m_media_fps));
        m_seekTimer->start(50);
    });

    connect(m_seekTimer, &QTimer::timeout, this, [this](){
        emit setPositionRequested(m_slider->value());
    });

    QHBoxLayout* buttonLayout =  new QHBoxLayout();

    // ------- test mute btn, hover et toggle
    QWidget* volumeSliderContainer = new QWidget();
    QVBoxLayout* volumeSliderLayout = new QVBoxLayout(volumeSliderContainer);
    volumeSliderLayout->setContentsMargins(0, 0, 0, 0);
    QSlider* volumeSlider = new QSlider(Qt::Vertical, volumeSliderContainer);
    volumeSlider->setFixedSize(20,100);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);
    volumeSliderLayout->addWidget(volumeSlider);

    m_muteBtn = new ToolbarToggleHoverButton(
        this, 
        volumeSliderContainer,
        false,
        "sound_off.png",  
        TextManager::instance().get("tooltip_sound_off"),
        "sound_on.png",
        TextManager::instance().get("tooltip_sound_on")
    );

    
    // ------- test speed btn, click show popup
    QWidget* speedSliderContainer = new QWidget();
    QVBoxLayout* speedSliderLayout = new QVBoxLayout(speedSliderContainer);
    speedSliderLayout->setContentsMargins(0, 0, 0, 0);
    QSlider* speedSlider = new QSlider(Qt::Horizontal, speedSliderContainer);
    speedSlider->setFixedSize(200,20);
    speedSlider->setRange(0, 6);
    speedSlider->setValue(3);
    speedSlider->setTickPosition(QSlider::TicksAbove);
    speedSlider->setTickInterval(1);

    speedSliderLayout->addWidget(speedSlider);

    m_speedBtn = new ToolbarPopupButton(this, speedSliderContainer, "speed.png",  TextManager::instance().get("tooltip_speed"));

    m_loopBtn = new ToolbarToggleButton(
        this,
        true,
        "loop_on.png",
        TextManager::instance().get("tooltip_loop_on"),
        "loop_off.png", 
        TextManager::instance().get("tooltip_loop_off")
    );

    m_removePlayerBtn = new ToolbarButton(this, "delete.png", TextManager::instance().get("tooltip_delete_player"));
    m_duplicatePlayerBtn = new ToolbarButton(this, "duplicate.png", TextManager::instance().get("tooltip_duplicate_player"));

    connect(m_duplicatePlayerBtn, &ToolbarButton::clicked, this,  &SimpleToolbar::duplicatePlayerRequested);
    connect(m_removePlayerBtn, &ToolbarButton::clicked, this, &SimpleToolbar::removePlayerRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateActivated, this, &SimpleToolbar::enableMuteRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateDeactivated, this, &SimpleToolbar::disableMuteRequest);
    connect(volumeSlider, &QSlider::valueChanged, this, &SimpleToolbar::volumeChanged);
    connect(speedSlider, &QSlider::valueChanged, this, &SimpleToolbar::speedChanged);
    connect(m_loopBtn, &ToolbarToggleButton::stateActivated, this, &SimpleToolbar::enableLoopModeRequest);
    connect(m_loopBtn, &ToolbarToggleButton::stateDeactivated, this, &SimpleToolbar::disableLoopModeRequest);

    setDefaultUI();
}


void SimpleToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    // Créer un layout quand on est en fullscreen
}


void SimpleToolbar::setDefaultUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* timecodeLayout = new QHBoxLayout();
    timecodeLayout->addWidget(m_currentTimeLabel, 1);
    timecodeLayout->addWidget(m_durationLabel, 0);
    mainLayout->addLayout(timecodeLayout);

    mainLayout->addWidget(m_slider);

    QHBoxLayout* buttonLayout = new QHBoxLayout();   
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_speedBtn);
    //buttonLayout->addWidget(m_slowDownBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    //buttonLayout->addWidget(m_speedUpBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    buttonLayout->addWidget(m_duplicatePlayerBtn);
    buttonLayout->addWidget(m_removePlayerBtn);
    mainLayout->addLayout(buttonLayout);

}

void SimpleToolbar::resetSlider()
{
    m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(0,1));
    m_durationLabel->setText(TimeFormatter::msToHHMMSSFF(0,1));
    m_slider->setRange(0,0);
    m_slider->setValue(0);
}

void SimpleToolbar::stopSlider()
{
    m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(0,m_media_fps));
    m_slider->setValue(0);
}

void SimpleToolbar::updateSliderRange(int64_t mediaDuration){

    Q_ASSERT(mediaDuration < static_cast<int64_t>(std::numeric_limits<int>::max()));

    if (mediaDuration >= static_cast<int64_t>(std::numeric_limits<int>::max()))
    {
        m_slider->setMaximum(std::numeric_limits<int>::max());
    }else {
        m_slider->setMaximum(mediaDuration);
    }
    m_durationLabel->setText(TimeFormatter::msToHHMMSSFF(mediaDuration, m_media_fps));


}

void SimpleToolbar::updateSliderValue(int64_t currentTime){
    
    if( m_draggingSlider) return;

    Q_ASSERT(currentTime < static_cast<int64_t>(std::numeric_limits<int>::max()));

    if (currentTime >= static_cast<int64_t>(std::numeric_limits<int>::max()))
    {
        m_slider->setValue(static_cast<int64_t>(std::numeric_limits<int>::max()));
    }

    m_slider->setValue(currentTime);
    m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(currentTime, m_media_fps));

}

void SimpleToolbar::updateFps(double newFps){
    m_media_fps = newFps;
}

void SimpleToolbar::playUiUpdate()
{
    m_playPauseBtn->setButtonState(true);
}

void SimpleToolbar::pauseUiUpdate()
{
    m_playPauseBtn->setButtonState(false);
}

void SimpleToolbar::muteUiUpdate()
{
    m_muteBtn->setButtonState(true);
}

void SimpleToolbar::unmuteUiUpdate()
{
    m_muteBtn->setButtonState(false);
}

void SimpleToolbar::ejectUiUpdate()
{
    resetSlider();
    pauseUiUpdate();
}

void SimpleToolbar::stopUiUpdate()
{
    stopSlider();
    pauseUiUpdate();
}

void SimpleToolbar::enableLoopUiUpdate()
{
    m_loopBtn->setButtonState(true);
}

void SimpleToolbar::disableLoopUiUpdate()
{
    m_loopBtn->setButtonState(false);
}
