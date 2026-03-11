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
#include <QGraphicsDropShadowEffect>

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    m_currentTimeLabel = new QLabel("00:00:00", this);
    m_durationLabel = new QLabel("00:00:00", this);
    m_nameLabel = new QLabel("", this);

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

    // QHBoxLayout* buttonLayout =  new QHBoxLayout();

    // ------- test mute btn, hover et toggle
    QWidget* volumeSliderContainer = new QWidget();

    volumeSliderContainer->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    volumeSliderContainer->setAttribute(Qt::WA_TranslucentBackground);

    QFrame* volumeSliderBackground = new QFrame(volumeSliderContainer);
    volumeSliderBackground->setStyleSheet(
        "QFrame{ background-color: palette(base); border-radius: 6px; border-style: solid; border-color: lightgray; border-width: 1px;}"
        "QLabel{ border: none; }"
    );

    QVBoxLayout* volumeContainerLayout = new QVBoxLayout(volumeSliderContainer);
    volumeContainerLayout->setContentsMargins(0,0,0,0);
    volumeContainerLayout->addWidget(volumeSliderBackground);

    QVBoxLayout* volumeFrameLayout = new QVBoxLayout(volumeSliderBackground);
    volumeFrameLayout->setContentsMargins(6,6,6,6);

    m_volumeSlider = new QSlider(Qt::Vertical);
    m_volumeSlider->setRange(0,100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->adjustSize();

    m_volumeLabel = new QLabel("100%");
    volumeFrameLayout->addWidget(m_volumeLabel);
    volumeFrameLayout->addWidget(m_volumeSlider);

    volumeFrameLayout->setAlignment(m_volumeSlider, Qt::AlignHCenter);
    volumeFrameLayout->setAlignment(m_volumeLabel, Qt::AlignHCenter);

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

    speedSliderContainer->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    speedSliderContainer->setAttribute(Qt::WA_TranslucentBackground);

    QFrame* speedSliderBackground = new QFrame(speedSliderContainer);
    speedSliderBackground->setStyleSheet(
        "QFrame {"
        " background-color: palette(base);"
        " border-radius: 6px;"
        " border: 1px solid lightgray;"
        "}"
        "QLabel {"
        " border: none;"
        "}"
        );

    QVBoxLayout* speedContainerLayout = new QVBoxLayout(speedSliderContainer);
    speedContainerLayout->setContentsMargins(0,0,0,0);
    speedContainerLayout->addWidget(speedSliderBackground);

    QVBoxLayout* speedFrameLayout = new QVBoxLayout(speedSliderBackground);
    speedFrameLayout->setContentsMargins(6,6,6,6);

    QHBoxLayout* speedInfoLayout = new QHBoxLayout();
    speedFrameLayout->addLayout(speedInfoLayout);
    m_speedLabel = new QLabel("x1");
    QLabel* slowIcon = new QLabel();
    slowIcon->setPixmap(QPixmap(":/icons/slow.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* fastIcon = new QLabel();
    fastIcon->setPixmap(QPixmap(":/icons/speed.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    speedInfoLayout->addWidget(slowIcon);
    speedInfoLayout->addStretch();
    speedInfoLayout->addWidget(m_speedLabel);
    speedInfoLayout->addStretch();
    speedInfoLayout->addWidget(fastIcon);

    QSlider* speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(0,6);
    speedSlider->setValue(3);
    speedSlider->setTickPosition(QSlider::TicksAbove);
    speedSlider->setTickInterval(1);

    speedFrameLayout->addWidget(speedSlider);

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
    timecodeLayout->addWidget(m_currentTimeLabel, 1, Qt::AlignLeft);
    timecodeLayout->addWidget(m_nameLabel, 1, Qt::AlignCenter);
    timecodeLayout->addWidget(m_durationLabel, 1, Qt::AlignRight);
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
    buttonLayout->addWidget(m_langBtn);
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
    
    m_discardVlcUiUpdates = false;

    if (mediaDuration >= static_cast<int64_t>(std::numeric_limits<int>::max()))
    {
        m_slider->setMaximum(std::numeric_limits<int>::max());
    }else {
        m_slider->setMaximum(mediaDuration);
    }
    m_durationLabel->setText(TimeFormatter::msToHHMMSSFF(mediaDuration, m_media_fps));


}

void SimpleToolbar::updateSliderValue(int64_t currentTime){
    
    if( m_draggingSlider || m_discardVlcUiUpdates) return;

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
    m_discardVlcUiUpdates = true;
    m_nameLabel->setText("");
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

void SimpleToolbar::nameUiUpdate(const QString & mediaName)
{
    m_nameLabel->setText(mediaName);
}

void SimpleToolbar::volumeUiUpdate(const QString & newVolume)
{
    m_volumeLabel->setText(newVolume + "%");
}

void SimpleToolbar::speedUiUpdate(const QString & newSpeed)
{
    m_speedLabel->setText("x" + newSpeed);
}

void SimpleToolbar::disableLoopMode()
{
    emit m_loopBtn->stateDeactivated();
}
