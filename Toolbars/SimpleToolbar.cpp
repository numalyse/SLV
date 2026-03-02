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

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    m_currentTimeLabel = new QLabel("00:00:00", this);
    m_durationLabel = new QLabel("00:00:00", this);

    m_slider = new QSlider(Qt::Horizontal, this);

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
    QWidget* containerTest = new QWidget();
    QVBoxLayout* mainLayoutTest = new QVBoxLayout(containerTest);
    mainLayoutTest->setContentsMargins(0, 0, 0, 0);
    QSlider* testWidget = new QSlider(Qt::Vertical, containerTest);
    testWidget->setFixedSize(20,100);
    mainLayoutTest->addWidget(testWidget);

    m_muteBtn = new ToolbarToggleHoverButton(
        this, 
        containerTest, 
        false,
        "sound_on.png",  
        TextManager::instance().get("tooltip_sound_on"),
        "sound_off.png",  
        TextManager::instance().get("tooltip_sound_off")
    );

    
    // ------- test speed btn, click show popup
    QWidget* containerTest2 = new QWidget();
    QVBoxLayout* mainLayoutTest2 = new QVBoxLayout(containerTest2);
    mainLayoutTest2->setContentsMargins(0, 0, 0, 0);
    QSlider* testWidget2 = new QSlider(Qt::Horizontal, containerTest2);
    testWidget2->setFixedSize(200,20);
    mainLayoutTest2->addWidget(testWidget2);

    m_speedBtn = new ToolbarPopupButton(this, containerTest2, "speed.png",  TextManager::instance().get("tooltip_speed"));

    // exemple pour connecter le slider dans le widget popup connect(testWidget, &QSlider::valueChanged, this, [&] () { qDebug() << "oui"; });

    //m_slowDownBtn = new ToolbarButton(this, "slow_down.png", TextManager::instance().get("tooltip_slow_down"));
    
    // m_playPauseBtn = new ToolbarToggleButton(
    //     this,
    //     false,
    //     "pause.png",
    //     TextManager::instance().get("tooltip_pause"),
    //     "play.png",
    //     TextManager::instance().get("tooltip_play")
    // );

    //m_speedUpBtn = new ToolbarButton(this, "speed_up.png", TextManager::instance().get("tooltip_speed_up"));

    m_loopBtn = new ToolbarToggleButton(
        this, 
        false,
        "loop_on.png", 
        TextManager::instance().get("tooltip_loop_on"),
        "loop_off.png", 
        TextManager::instance().get("tooltip_loop_off")
    );

    m_removePlayerBtn = new ToolbarButton(this, "delete.png", TextManager::instance().get("tooltip_delete_player"));;
    connect(m_removePlayerBtn, &ToolbarButton::clicked, this, &SimpleToolbar::removePlayerRequest);

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
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    buttonLayout->addWidget(m_removePlayerBtn);
    mainLayout->addLayout(buttonLayout);

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

void SimpleToolbar::updateFps(float newFps){
    m_media_fps = newFps;
}