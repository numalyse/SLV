#include "SimpleToolbar.h"
#include "TextManager.h"
#include "ToolbarButton.h"
#include "ToolbarToggleButton.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    m_containerWidget = new QWidget(this);

    m_currentTimeLabel = new QLabel("00:00:00");
    m_durationLabel = new QLabel("00:00:00");

    m_slider = new QSlider(Qt::Horizontal, m_containerWidget);

    QHBoxLayout* buttonLayout =  new QHBoxLayout();

    m_muteBtn = new ToolbarToggleButton(
        m_containerWidget, 
        false,
        "sound_on.png", 
        TextManager::instance().get("tooltip_sound_on"),
        "sound_off.png", 
        TextManager::instance().get("tooltip_sound_off")
    );
    

    m_soundSlider = new QSlider(Qt::Vertical, m_containerWidget);
    m_soundSlider->setFixedSize(20,100);
    m_slowDownBtn = new ToolbarButton(m_containerWidget, "slow_down.png", TextManager::instance().get("tooltip_slow_down"));
    
    m_playPauseBtn = new ToolbarToggleButton(
        m_containerWidget, 
        false,
        "pause.png", 
        TextManager::instance().get("tooltip_pause"),
        "play.png", 
        TextManager::instance().get("tooltip_play")
    );

    m_speedUpBtn = new ToolbarButton(m_containerWidget, "speed_up.png", TextManager::instance().get("tooltip_speed_up"));
    m_stopBtn = new ToolbarButton(m_containerWidget, "stop.png", TextManager::instance().get("tooltip_stop"));
    m_ejectBtn = new ToolbarButton(m_containerWidget, "eject.png", TextManager::instance().get("tooltip_eject"));
    m_fullscreenBtn = new ToolbarButton(m_containerWidget, "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"));
    m_loopBtn = new ToolbarButton(m_containerWidget, "loop_off", TextManager::instance().get("tooltip_loop_off"));


    setDefaultUI();
}


void SimpleToolbar::setFullscreenUI()
{
    if (m_containerWidget->layout() != nullptr) {
        delete m_containerWidget->layout();
    }

    // Créer un layout quand on est en fullscreen
}

void SimpleToolbar::setDefaultUI()
{
    if (m_containerWidget->layout() != nullptr) {
        delete m_containerWidget->layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* timecodeLayout = new QHBoxLayout();
    timecodeLayout->addWidget(m_currentTimeLabel, 1);
    timecodeLayout->addWidget(m_durationLabel, 0);
    mainLayout->addLayout(timecodeLayout);

    mainLayout->addWidget(m_slider);

    QHBoxLayout* buttonLayout =  new QHBoxLayout();   
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_soundSlider);
    buttonLayout->addWidget(m_slowDownBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_speedUpBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    mainLayout->addLayout(buttonLayout);

    this->addWidget(m_containerWidget);
}
