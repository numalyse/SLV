#include "SimpleToolbar.h"
#include "TextManager.h"
#include "ToolbarButton.h"
#include "ToolbarToggleButton.h"
#include "ToolbarPopupButton.h" 

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    m_currentTimeLabel = new QLabel("00:00:00", this);
    m_durationLabel = new QLabel("00:00:00", this);

    m_slider = new QSlider(Qt::Horizontal, this);

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

    m_slowDownBtn = new ToolbarButton(this, "slow_down.png", TextManager::instance().get("tooltip_slow_down"));
    
    m_playPauseBtn = new ToolbarToggleButton(
        this, 
        false,
        "pause.png", 
        TextManager::instance().get("tooltip_pause"),
        "play.png", 
        TextManager::instance().get("tooltip_play")
    );

    m_speedUpBtn = new ToolbarButton(this, "speed_up.png", TextManager::instance().get("tooltip_speed_up"));
    m_stopBtn = new ToolbarButton(this, "stop.png", TextManager::instance().get("tooltip_stop"));
    m_ejectBtn = new ToolbarButton(this, "eject.png", TextManager::instance().get("tooltip_eject"));
    m_fullscreenBtn = new ToolbarButton(this, "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"));

    m_loopBtn = new ToolbarToggleButton(
        this, 
        false,
        "loop_on.png", 
        TextManager::instance().get("tooltip_loop_on"),
        "loop_off.png", 
        TextManager::instance().get("tooltip_loop_off")
    );

    setDefaultUI();
}

/// @brief Met à jour le layout du slider pour afficher l'interface en plein écran
void SimpleToolbar::setFullscreenUI()
{
    if (this->layout() != nullptr) {
        delete this->layout();
    }

    // Créer un layout quand on est en fullscreen
}

/// @brief Met à jour le layout du slider pour afficher l'interface par défaut
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
    buttonLayout->addWidget(m_slowDownBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_speedUpBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    mainLayout->addLayout(buttonLayout);

}
