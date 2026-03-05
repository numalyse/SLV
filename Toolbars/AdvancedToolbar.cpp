#include "Toolbars/AdvancedToolbar.h"

#include "TextManager.h"

#include "Toolbars/ExtensionToolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"
#include "AdvancedToolbar.h"


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

/// @brief Constructeur qui va copier les états des boutons de le toolbar passé en paramète 
/// @param parent 
/// @param toolbar 
AdvancedToolbar::AdvancedToolbar(QWidget *parent, SimpleToolbar *toolbar) 
    : AdvancedToolbar(parent)
{
    if (!toolbar) return;

    m_media_fps = toolbar->mediaFps();

    QSlider* oldSlider = toolbar->slider();
    if (oldSlider && m_slider) {
        m_slider->setRange(oldSlider->minimum(), oldSlider->maximum());
        m_slider->setValue(oldSlider->value());
    }

    QLabel* oldTimeLabel = toolbar->currentTimeLabel();
    if (oldTimeLabel && m_currentTimeLabel) {
        m_currentTimeLabel->setText(oldTimeLabel->text());
    }

    QLabel* oldDurationLabel = toolbar->durationLabel();
    if (oldDurationLabel && m_durationLabel) {
        m_durationLabel->setText(oldDurationLabel->text());
    }

    ToolbarToggleButton* oldPlayPause = toolbar->playPauseBtn();
    if (oldPlayPause && m_playPauseBtn) {
        m_playPauseBtn->setButtonState(oldPlayPause->isChecked());
    }

    ToolbarToggleButton* oldFullscreen = toolbar->fullscreenBtn();
    if (oldFullscreen && m_fullscreenBtn) {
        m_fullscreenBtn->setButtonState(oldFullscreen->isChecked());
    }

    ToolbarToggleButton* oldLoop = toolbar->loopBtn();
    if (oldLoop && m_loopBtn) {
        m_loopBtn->setButtonState(oldLoop->isChecked());
    }

    ToolbarToggleHoverButton* oldMute = toolbar->muteBtn();
    if (oldMute && m_muteBtn) {
        m_muteBtn->setButtonState(oldMute->isChecked());
    }

    // TODO : voir pour copier les états des sliders dans muteBtn et speedBtn

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
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_loopBtn);
    buttonLayout->addWidget(m_duplicatePlayerBtn);
    buttonLayout->addWidget(m_extensionBtn);

    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(m_extensionToolbar);

}
