#include "Toolbars/AdvancedToolbar.h"

#include "TextManager.h"

#include "Toolbars/ExtensionToolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"


AdvancedToolbar::AdvancedToolbar(QWidget *parent) : SimpleToolbar(parent)
{
    m_nextMediaBtn = new ToolbarButton(this, "next.png", TextManager::instance().get("tooltip_next_media"));
    m_prevMediaBtn = new ToolbarButton(this, "prev.png", TextManager::instance().get("tooltip_prev_media"));

    m_extensionBtn = new ToolbarToggleButton(this,
        false,
        "minus.png",
        TextManager::instance().get("tooltip_expand_toolbar"),
        "plus.png",
        TextManager::instance().get("tooltip_minimize_toolbar")
    );

    m_extensionToolbar = new ExtensionToolbar(this);

    connect(m_extensionBtn, &ToolbarToggleButton::stateActivated, m_extensionToolbar, &QWidget::show);
    connect(m_extensionBtn, &ToolbarToggleButton::stateDeactivated, m_extensionToolbar, &QWidget::hide);

    delete m_removePlayerBtn; // On ne veut pas de ce bouton dans cette toolbar
    m_removePlayerBtn = nullptr;

    setDefaultUI();
}

void AdvancedToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }
    // Créer un layout quand on est en fullscreen
}

void AdvancedToolbar::setDefaultUI()
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
    buttonLayout->addWidget(m_prevMediaBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_nextMediaBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    buttonLayout->addWidget(m_extensionBtn);

    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(m_extensionToolbar);

}
