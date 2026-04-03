#include "Toolbars/GlobalToolbar.h"

#include "PrefManager.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


GlobalToolbar::GlobalToolbar(QWidget *parent) : Toolbar(parent)
{
    m_muteBtn = new ToolbarToggleButton(
        this,
        false,
        "sound_off_white",
        PrefManager::instance().getText("tooltip_sound_on"),
        "sound_on_white",
        PrefManager::instance().getText("tooltip_sound_off")
    );
    m_muteBtn->setEnabled(true);

    connect(m_muteBtn, &ToolbarToggleButton::stateActivated, this, &GlobalToolbar::enableMute);
    connect(m_muteBtn, &ToolbarToggleButton::stateDeactivated, this, &GlobalToolbar::disableMute);
    // connect(m_screenshotBtn, &ToolbarButton::clicked, this, &GlobalToolbar::screenshotRequest);

    setMaximumHeight(50);

    setDefaultUI();
    // disableButtons();

    addShortcuts();
}

void GlobalToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    // Créer un layout quand on est en fullscreen
}

void GlobalToolbar::enableFullscreenUiUpdate()
{
    m_fullscreenBtn->setButtonState(true);
}

void GlobalToolbar::disableFullscreenUiUpdate()
{
    m_fullscreenBtn->setButtonState(false);
}

void GlobalToolbar::setDefaultUI()
{

    if (layout() != nullptr) {
        delete layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(5,0,5,0);
    buttonLayout->setSpacing(1);
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addSpacing(m_muteBtn->width());
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_fullscreenBtn);

    mainLayout->addLayout(buttonLayout);

}

void GlobalToolbar::enableButtons()
{
    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    m_ejectBtn->setEnabled(true);
    // m_muteBtn->setEnabled(true);
    m_fullscreenBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
}

void GlobalToolbar::disableButtons()
{
    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_ejectBtn->setEnabled(false);
    // m_muteBtn->setEnabled(false);
    m_fullscreenBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
}

void GlobalToolbar::addShortcuts(){
    auto& prefManager = PrefManager::instance();
    QJsonObject commonShortcuts = prefManager.getSubCategory("Shortcuts", "CommonToolbar");
    
    m_playPauseBtn->setShortcut(QKeySequence(commonShortcuts.value("play_pause").toString()));
    m_stopBtn->setShortcut(QKeySequence(commonShortcuts.value("stop").toString()));
    m_fullscreenBtn->setShortcut(QKeySequence(commonShortcuts.value("enter_fullscreen").toString()));
    m_muteBtn->setShortcut(QKeySequence(commonShortcuts.value("mute").toString()));
    m_screenshotBtn->setShortcut(QKeySequence(commonShortcuts.value("capture").toString()));
}

void GlobalToolbar::disableFullscreenRequested(){
    m_fullscreenBtn->setShortcut(QKeySequence(PrefManager::instance().getPref("Shortcuts", "CommonToolbar" , "enter_fullscreen")));
    emit disableFullscreenRequest();
}
