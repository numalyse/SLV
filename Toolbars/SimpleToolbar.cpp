#include "Toolbars/SimpleToolbar.h"
#include "TextManager.h"
#include "TimeFormatter.h"
#include "SignalManager.h"

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
        m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(m_slider->value(), m_media_fps));
        emit setPositionRequested(m_slider->value());
        emit simpleToolbarUpdateCursorPosition(m_slider->value());
    });

    connect(this, &SimpleToolbar::simpleToolbarUpdateCursorPosition, &SignalManager::instance(), &SignalManager::simpleToolbarUpdateCursorPosition);

    connect(m_slider, &QSlider::sliderReleased, this, [this]() {
        m_draggingSlider = false;
    });

    connect(m_slider, &QSlider::sliderMoved, this, [this](){
        m_currentTimeLabel->setText(TimeFormatter::msToHHMMSSFF(m_slider->value(), m_media_fps));
        emit simpleToolbarUpdateCursorPosition(m_slider->value());
        m_seekTimer->start(50);
    });

    connect(m_seekTimer, &QTimer::timeout, this, [this](){
        emit setPositionRequested(m_slider->value());
    });

    // QHBoxLayout* buttonLayout =  new QHBoxLayout();

    QVBoxLayout* volumeFrameLayout = new QVBoxLayout();
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
        volumeFrameLayout,
        false,
        "sound_off_white",
        TextManager::instance().get("tooltip_sound_off"),
        "sound_on_white",
        TextManager::instance().get("tooltip_sound_on")
    );
    m_muteBtn->setEnabled(true);
    
    QVBoxLayout* speedFrameLayout = new QVBoxLayout();
    speedFrameLayout->setContentsMargins(6,6,6,6);

    QHBoxLayout* speedInfoLayout = new QHBoxLayout();
    speedFrameLayout->addLayout(speedInfoLayout);
    m_speedLabel = new QLabel("x1");
    QLabel* slowIcon = new QLabel();
    slowIcon->setPixmap(QPixmap(":/icons/slow_white").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* fastIcon = new QLabel();
    fastIcon->setPixmap(QPixmap(":/icons/speed_white").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    m_speedBtn = new ToolbarToggleHoverButton(this, speedFrameLayout, false, "speed_white",  TextManager::instance().get("tooltip_speed"), "speed_white", TextManager::instance().get("tooltip_speed"));

    m_loopBtn = new ToolbarToggleButton(
        this,
        true,
        "loop_on.png",
        TextManager::instance().get("tooltip_loop_on"),
        "loop_off_white",
        TextManager::instance().get("tooltip_loop_off")
    );
    QVBoxLayout* langLayout = new QVBoxLayout();
    m_langBtn = new ToolbarPopupButton(this, langLayout, "lang_white", TextManager::instance().get("tooltip_lang"));

    m_removePlayerBtn = new ToolbarButton(this, "remove_media_white", TextManager::instance().get("tooltip_delete_player"));
    m_duplicatePlayerBtn = new ToolbarButton(this, "duplicate_media_white", TextManager::instance().get("tooltip_duplicate_player"));

    connect(m_duplicatePlayerBtn, &ToolbarButton::clicked, this,  &SimpleToolbar::duplicatePlayerRequested);
    connect(m_removePlayerBtn, &ToolbarButton::clicked, this, &SimpleToolbar::removePlayerRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateActivated, this, &SimpleToolbar::enableMuteRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateDeactivated, this, &SimpleToolbar::disableMuteRequest);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &SimpleToolbar::volumeChanged);
    connect(speedSlider, &QSlider::valueChanged, this, &SimpleToolbar::speedChanged);
    connect(m_loopBtn, &ToolbarToggleButton::stateActivated, this, &SimpleToolbar::enableLoopModeRequest);
    connect(m_loopBtn, &ToolbarToggleButton::stateDeactivated, this, &SimpleToolbar::disableLoopModeRequest);

    setDefaultUI();
    disableButtons();
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
    mainLayout->setContentsMargins(5,5,5,5);
    mainLayout->setSpacing(1);

    QHBoxLayout* timecodeLayout = new QHBoxLayout();
    timecodeLayout->addWidget(m_currentTimeLabel, 1, Qt::AlignLeft);
    timecodeLayout->addWidget(m_nameLabel, 1, Qt::AlignCenter);
    timecodeLayout->addWidget(m_durationLabel, 1, Qt::AlignRight);
    mainLayout->addLayout(timecodeLayout);

    mainLayout->addWidget(m_slider);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(1);
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_langBtn);
    buttonLayout->addSpacing(m_langBtn->width());
    buttonLayout->addSpacing(m_langBtn->width());
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_speedBtn);
    buttonLayout->addWidget(m_stopBtn);
    //buttonLayout->addWidget(m_slowDownBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    //buttonLayout->addWidget(m_speedUpBtn);

    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_loopBtn);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_duplicatePlayerBtn);
    buttonLayout->addWidget(m_removePlayerBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
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

    emit setCursorPositionRequested(currentTime);

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
    QPalette p = m_volumeSlider->palette();
    p.setColor(QPalette::Highlight, Qt::darkGray);
    m_volumeSlider->setPalette(p);
}

void SimpleToolbar::unmuteUiUpdate()
{
    m_muteBtn->setButtonState(false);
    m_volumeSlider->setStyleSheet("");

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

void SimpleToolbar::enableButtons()
{
    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    m_ejectBtn->setEnabled(true);
    // m_muteBtn->setEnabled(true);
    m_langBtn->setEnabled(true);
    m_loopBtn->setEnabled(true);
    m_duplicatePlayerBtn->setEnabled(true);
    m_removePlayerBtn->setEnabled(true);
    // m_speedBtn->setEnabled(true);
    m_fullscreenBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
}

void SimpleToolbar::disableButtons()
{
    m_stopBtn->setEnabled(false);
    m_ejectBtn->setEnabled(false);
    // m_muteBtn->setEnabled(false);
    m_langBtn->setEnabled(false);
    m_loopBtn->setEnabled(false);
    m_duplicatePlayerBtn->setEnabled(false);
    m_removePlayerBtn->setEnabled(false);
    // m_speedBtn->setEnabled(false);
    m_fullscreenBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
}
