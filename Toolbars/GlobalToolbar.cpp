#include "Toolbars/GlobalToolbar.h"

#include "PrefManager.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarButton.h"
#include "ShortcutHelper.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include "GlobalToolbar.h"


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
    Toolbar::setDefaultUI();

    delete layout();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);

    QGridLayout* buttonLayout = new QGridLayout();
    buttonLayout->setContentsMargins(5,5,5,5);
    buttonLayout->setSpacing(0);

    QHBoxLayout* leftLayout = new QHBoxLayout();
    leftLayout->setSpacing(1);
    leftLayout->addWidget(m_muteBtn);

    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->setSpacing(1);
    centerLayout->addWidget(m_stopBtn);
    centerLayout->addWidget(m_playPauseBtn);
    centerLayout->addWidget(m_ejectBtn);


    QHBoxLayout* rightLayout = new QHBoxLayout();
    rightLayout->setSpacing(1);
    rightLayout->addWidget(m_zoomBtn);
    rightLayout->addWidget(m_screenshotBtn);
    rightLayout->addWidget(m_fullscreenBtn);

    buttonLayout->addLayout(leftLayout, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    buttonLayout->addLayout(centerLayout, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    buttonLayout->addLayout(rightLayout, 0, 2, Qt::AlignRight | Qt::AlignVCenter);

    buttonLayout->setColumnStretch(0, 1); 
    buttonLayout->setColumnStretch(1, 0); 
    buttonLayout->setColumnStretch(2, 1); 

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
    
    addEnterFullscreenShortcut();

    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("play_pause").toString(), m_playPauseBtn));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("stop").toString(), m_stopBtn,  false));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("mute").toString(), m_muteBtn,  false));
    m_globalShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("screenshot").toString(), m_screenshotBtn,  false));
}

void GlobalToolbar::disableFullscreenRequested(){
    addEnterFullscreenShortcut();
    emit disableFullscreenRequest();
}
