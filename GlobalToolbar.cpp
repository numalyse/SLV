#include "GlobalToolbar.h"

#include "TextManager.h"
#include "ToolbarToggleButton.h"
#include "ToolbarButton.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


GlobalToolbar::GlobalToolbar(QWidget *parent)
{
    m_containerWidget = new QWidget(this);

    QHBoxLayout* buttonLayout =  new QHBoxLayout();

    m_playPauseBtn = new ToolbarToggleButton(
        m_containerWidget, 
        false,
        "pause.png", 
        TextManager::instance().get("tooltip_pause"),
        "play.png", 
        TextManager::instance().get("tooltip_play")
    );

    m_stopBtn = new ToolbarButton(m_containerWidget, "stop.png", TextManager::instance().get("tooltip_stop"));
    m_ejectBtn = new ToolbarButton(m_containerWidget, "eject.png", TextManager::instance().get("tooltip_eject"));
    m_fullscreenBtn = new ToolbarButton(m_containerWidget, "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"));

    setDefaultUI();
}

void GlobalToolbar::setFullscreenUI()
{
    if (m_containerWidget->layout() != nullptr) {
        delete m_containerWidget->layout();
    }

    // Créer un layout quand on est en fullscreen
}

void GlobalToolbar::setDefaultUI()
{

    if (m_containerWidget->layout() != nullptr) {
        delete m_containerWidget->layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    QHBoxLayout* buttonLayout = new QHBoxLayout();   
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    mainLayout->addLayout(buttonLayout);

    this->addWidget(m_containerWidget);
}

