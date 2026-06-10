#include "Toolbars/GlobalToolbar.h"

#include "PrefManager.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarButton.h"
#include "ShortcutHelper.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>

#include "GlobalToolbar.h"


GlobalToolbar::GlobalToolbar(QWidget *parent) : Toolbar(parent)
{
    m_muteBtn = new ToolbarToggleButton(
        this,
        false,
        "sound_off_white",
        PrefManager::instance().getText("tooltip_sound_on") + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "mute") + ")</i>",
        "sound_on_white",
        PrefManager::instance().getText("tooltip_sound_off") + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "mute") + ")</i>"
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

GlobalToolbar::~GlobalToolbar()
{
    SLV::clearShortcuts(m_globalShortcuts);
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
    m_zoomBtn->show();

    Toolbar::setDefaultUI();

    if( !layout() ){
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0,0,0,0);
        mainLayout->setSpacing(1);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->setContentsMargins(5,5,5,5);
        buttonLayout->setSpacing(1);
        
        buttonLayout->addWidget(m_muteBtn);
        buttonLayout->addSpacing(m_muteBtn->width()+1);
        buttonLayout->addSpacing(m_muteBtn->width()+1);
        m_extraSpacingItem = new QSpacerItem(m_muteBtn->width()+1, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
        buttonLayout->addSpacerItem(m_extraSpacingItem);
        
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_stopBtn);
        buttonLayout->addWidget(m_playPauseBtn);
        buttonLayout->addWidget(m_ejectBtn);
        buttonLayout->addSpacing(m_ejectBtn->width()+1);

        buttonLayout->addStretch();
        buttonLayout->addWidget(m_zoomBtn);
        buttonLayout->addWidget(m_screenshotBtn);
        buttonLayout->addWidget(m_fullscreenBtn);

        mainLayout->addLayout(buttonLayout);
    } else if (m_extraSpacingItem) { // si on vient de quitter le mode plein écran, on a déja un layout
        m_extraSpacingItem->changeSize(m_muteBtn->width()+1, 1, QSizePolicy::Fixed, QSizePolicy::Minimum); // augmente la taille du spacer
        layout()->invalidate(); // pour forcer a recalculer les positions des btns 

    }
}

void GlobalToolbar::setFullscreenUI(int bottomMargin)
{

    m_zoomBtn->hide();

    Toolbar::setFullscreenUI(bottomMargin);

    if (layout() && m_extraSpacingItem) {
        m_extraSpacingItem->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
        layout()->invalidate(); // pour forcer a recalculer les positions des btns 
    }
}

void GlobalToolbar::enableButtons()
{
    Toolbar::enableButtons();

    m_zoomBtn->setEnabled(true);
}

void GlobalToolbar::disableButtons()
{
    Toolbar::disableButtons();

    m_zoomBtn->setEnabled(false);
}

void GlobalToolbar::addShortcuts(){
    auto& prefManager = PrefManager::instance();
    QJsonObject commonShortcuts = prefManager.getSubCategory("Shortcuts", "CommonToolbar");
    
    addEnterFullscreenShortcut();

    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("play_pause").toString(), m_playPauseBtn));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("stop").toString(), m_stopBtn,  false));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("mute").toString(), m_muteBtn,  false));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("screenshot").toString(), m_screenshotBtn,  false));

    // raccourcis "matériels"

    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaTogglePlayPause).toString(), m_playPauseBtn));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaPlay).toString(), m_playPauseBtn));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaPause).toString(), m_playPauseBtn));

    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_VolumeMute).toString(), m_muteBtn, false));
}

void GlobalToolbar::disableFullscreenRequested(){
    addEnterFullscreenShortcut();
    emit disableFullscreenRequest();
}

void GlobalToolbar::updateFullscreenPosition()
{
    Toolbar::moveOnTopOfParent(GlobalToolbar::s_bottomMarginFullscreen);
}