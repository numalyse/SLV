#include "Toolbars/GlobalToolbar.h"

#include "TextManager.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


GlobalToolbar::GlobalToolbar(QWidget *parent) : Toolbar(parent)
{
    m_muteBtn = new ToolbarToggleButton(
        this,
        false,
        "sound_off.png",
        TextManager::instance().get("tooltip_sound_off"),
        "sound_on.png",
        TextManager::instance().get("tooltip_sound_on")
    );

    connect(m_muteBtn, &ToolbarToggleButton::stateActivated, this, &GlobalToolbar::enableMute);
    connect(m_muteBtn, &ToolbarToggleButton::stateDeactivated, this, &GlobalToolbar::disableMute);

    setMaximumHeight(50);

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
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    mainLayout->addLayout(buttonLayout);
}

