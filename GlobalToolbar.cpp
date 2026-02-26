#include "GlobalToolbar.h"

#include "TextManager.h"
#include "ToolbarToggleButton.h"
#include "ToolbarButton.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


GlobalToolbar::GlobalToolbar(QWidget *parent)
{
    QHBoxLayout* buttonLayout =  new QHBoxLayout();

    m_playPauseBtn = new ToolbarToggleButton(
        this, 
        false,
        "pause.png", 
        TextManager::instance().get("tooltip_pause"),
        "play.png", 
        TextManager::instance().get("tooltip_play")
    );

    m_stopBtn = new ToolbarButton(this, "stop.png", TextManager::instance().get("tooltip_stop"));
    m_ejectBtn = new ToolbarButton(this, "eject.png", TextManager::instance().get("tooltip_eject"));
    m_fullscreenBtn = new ToolbarButton(this, "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"));

    setDefaultUI();
}

void GlobalToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    // Créer un layout quand on est en fullscreen
}

void GlobalToolbar::setDefaultUI()
{

    if (layout() != nullptr) {
        delete layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    QHBoxLayout* buttonLayout = new QHBoxLayout();   
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    mainLayout->addLayout(buttonLayout);
}

