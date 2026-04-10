#include "Toolbars/ExtensionToolbar.h"
#include "Project/ProjectManager.h"

#include "PrefManager.h"
#include "ExtensionToolbar.h"
#include "OverlayMode.h"
#include "SignalManager.h"
#include "ShortcutHelper.h"

#include <QHBoxLayout>
#include <qframe.h>
#include <QComboBox>
#include <QCheckBox>
#include <QIcon>
#include <QVariant>
#include <QShortcut>


ExtensionToolbar::ExtensionToolbar(QWidget *parent) : QWidget(parent)
{

    m_zoomBtn = new ToolbarToggleButton(
        this, 
        false,
        "zoom_white",
        PrefManager::instance().getText("tooltip_zoom_on"),
        "zoom_white",
        PrefManager::instance().getText("tooltip_zoom_off")
    );

    m_hideImgBtn = new ToolbarToggleButton(
        this, 
        false,
        "hide_image_white",
        PrefManager::instance().getText("tooltip_hide_image"),
        "show_image_white",
        PrefManager::instance().getText("tooltip_show_image")
    );

    m_prevFrameBtn = new ToolbarButton(this, "prev_frame_white", PrefManager::instance().getText("tooltip_prev_frame"));
    m_nextFrameBtn = new ToolbarButton(this, "next_frame_white", PrefManager::instance().getText("tooltip_next_frame"));
    
    m_backwardBtn = new ToolbarButton(this, "backward_white", PrefManager::instance().getText("tooltip_backward"));
    m_forwardBtn = new ToolbarButton(this, "forward_white", PrefManager::instance().getText("tooltip_forward"));
    
    m_rotateBtn = new ToolbarButton(this, "rotate_white", PrefManager::instance().getText("tooltip_rotate"));

    m_recordBtn = new ToolbarToggleButton(
        this,
        false,
        "record_on.png",
        PrefManager::instance().getText("tooltip_record_on"),
        "record_off_white",
        PrefManager::instance().getText("tooltip_record_off")
        );
    m_recordBtn->setIconSize(QSize(30, 30));

    m_segmBtn = new ToolbarToggleButton(
        this, 
        false,
        "timeline_on_white",
        PrefManager::instance().getText("tooltip_segmentation_on"),
        "timeline_off_white",
        PrefManager::instance().getText("tooltip_segmentation_off")
    );

    connect(m_segmBtn, &ToolbarToggleButton::stateActivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        if( ProjectManager::instance().projet()){
            m_segmBtn->setButtonState(true);
            emit enableSegmentationRequested();
            emit SignalManager::instance().extensionToolbarDisplayShotDetail();
        }
    });

    connect(&ProjectManager::instance(), &ProjectManager::projectDeleted, this, [this] { // quand le projet est détruit, on force le button segmentation en false   
            m_segmBtn->setButtonState(false); 
    });

    connect(m_hideImgBtn, &ToolbarToggleButton::stateActivated, &SignalManager::instance(), &SignalManager::extendedToolbarHideImageEnabled);
    connect(m_hideImgBtn, &ToolbarToggleButton::stateDeactivated, &SignalManager::instance(), &SignalManager::extendedToolbarHideImageDisabled);

    connect(m_prevFrameBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::prevFrameRequested);
    connect(m_nextFrameBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::nextFrameRequested);
    
    connect(m_backwardBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::moveTimeBackwardRequested);
    connect(m_forwardBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::moveTimeForwardRequested);

    connect(m_rotateBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::rotateRequested);

    connect(m_recordBtn, &ToolbarToggleButton::stateActivated, this, &ExtensionToolbar::enableRecordRequested);
    connect(m_recordBtn, &ToolbarToggleButton::stateDeactivated, this, &ExtensionToolbar::disableRecordRequested);

    m_verticalInvBtn = new ToolbarToggleButton(this, false, "invert_v_on_white", PrefManager::instance().getText("tooltip_flip_vertical"), "invert_v_white", PrefManager::instance().getText("tooltip_flip_vertical"));
    m_horizontalInvBtn = new ToolbarToggleButton(this, false, "invert_h_on_white", PrefManager::instance().getText("tooltip_flip_horizontal"),  "invert_h_white", PrefManager::instance().getText("tooltip_flip_horizontal"));

    connect(m_horizontalInvBtn, &ToolbarToggleButton::clicked, this, &ExtensionToolbar::horizontalFlipRequested);
    connect(m_verticalInvBtn, &ToolbarToggleButton::clicked, this, &ExtensionToolbar::verticalFlipRequested);

    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, &ExtensionToolbar::disableSegmentationRequested);

    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        emit disableSegmentationRequested();
        m_segmBtn->setButtonState(false);
        emit SignalManager::instance().displayPlaylist();
    });
    connect(&SignalManager::instance(), &SignalManager::recordButtonUiUpdate, this, &ExtensionToolbar::updateRecordButtonUI);

    QHBoxLayout* compoRuleLayout = new QHBoxLayout();

    m_compoRuleComboBox = new QComboBox(this);
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_None"), QVariant::fromValue(OverlayMode::None));
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_RuleOfThirds"), QVariant::fromValue(OverlayMode::RuleOfThirds));
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_CenterCross"), QVariant::fromValue(OverlayMode::CenterCross));
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_Diagonals"), QVariant::fromValue(OverlayMode::Diagonals));
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_L_Shape"), QVariant::fromValue(OverlayMode::L_Shape));
    m_compoRuleComboBox->addItem(PrefManager::instance().getText("compo_rule_GoldenRatio"), QVariant::fromValue(OverlayMode::GoldenRatio));

    connect(m_compoRuleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExtensionToolbar::updateOverlayMode);

    compoRuleLayout->addWidget(m_compoRuleComboBox);

    m_compoRuleCheckboxHFlip = new QCheckBox(this); 
    m_compoRuleCheckboxVFlip = new QCheckBox(this);
    m_compoRuleCheckboxVFlip->setIcon(QIcon(":/icons/invert_v_white"));
    m_compoRuleCheckboxHFlip->setIcon(QIcon(":/icons/invert_h_white"));
    m_compoRuleCheckboxVFlip->setIconSize(QSize(20, 20));
    m_compoRuleCheckboxHFlip->setIconSize(QSize(20, 20));
    m_compoRuleCheckboxHFlip->setToolTip(PrefManager::instance().getText("tooltip_compo_rule_VFlip"));
    m_compoRuleCheckboxVFlip->setToolTip(PrefManager::instance().getText("tooltip_compo_rule_HFlip"));

    connect(m_compoRuleCheckboxVFlip, &QCheckBox::toggled, this, &ExtensionToolbar::updateOverlayMode);
    connect(m_compoRuleCheckboxHFlip, &QCheckBox::toggled, this, &ExtensionToolbar::updateOverlayMode);
    
    compoRuleLayout->addWidget(m_compoRuleCheckboxVFlip);
    compoRuleLayout->addWidget(m_compoRuleCheckboxHFlip);

    m_compoRuleBtn = new ToolbarPopupButton(this, compoRuleLayout, "compo_rule_white", PrefManager::instance().getText("tooltip_composition"));
   
    QHBoxLayout* invFrameLayout = new QHBoxLayout();

    invFrameLayout->addWidget(m_verticalInvBtn);
    invFrameLayout->addWidget(m_horizontalInvBtn);
    m_invBtn = new ToolbarPopupButton(this, invFrameLayout, "invert_h_white", PrefManager::instance().getText("tooltip_flip_vertical"));
    


    setDefaultUI();
    disableButtons();
    hide();
    addShortcuts();
}

ExtensionToolbar::~ExtensionToolbar()
{
    SLV::clearShortcuts(m_extensionShortcuts);
}

void ExtensionToolbar::updateOverlayMode(){
    auto mode = static_cast<OverlayMode>(m_compoRuleComboBox->currentIndex());
    emit setOverlayModeRequested(
        mode,
        m_compoRuleCheckboxVFlip->isChecked(),
        m_compoRuleCheckboxHFlip->isChecked()
    );
}

void ExtensionToolbar::setFullscreenUI()
{    
    if (layout() != nullptr) {
        delete layout();
    }
    // Créer un layout quand on est en fullscreen
}


void ExtensionToolbar::setDefaultUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(1);
    mainLayout->addStretch();

    mainLayout->addWidget(m_zoomBtn);
    mainLayout->addWidget(m_hideImgBtn);
    
    mainLayout->addWidget(m_backwardBtn);
    mainLayout->addWidget(m_prevFrameBtn);
    mainLayout->addWidget(m_nextFrameBtn);
    mainLayout->addWidget(m_forwardBtn);
    
    mainLayout->addWidget(m_rotateBtn);
    mainLayout->addWidget(m_recordBtn);
    mainLayout->addWidget(m_invBtn);
    // mainLayout->addWidget(m_abloopBtn);
    mainLayout->addWidget(m_segmBtn);
    mainLayout->addWidget(m_compoRuleBtn);
    mainLayout->addStretch();

}

void ExtensionToolbar::updateRecordButtonUI()
{
    m_recordBtn->toggleUpdateIcon();
}

void ExtensionToolbar::updateHFlipButtonUI()
{
    m_horizontalInvBtn->toggleUpdateIcon();
}

void ExtensionToolbar::updateVFlipButtonUI()
{
    m_verticalInvBtn->toggleUpdateIcon();
}


void ExtensionToolbar::addShortcuts()
{
    auto& prefManager = PrefManager::instance();
    QJsonObject extShortcuts = prefManager.getSubCategory("Shortcuts", "ExtensionTB");

    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("h_flip").toString(), m_horizontalInvBtn, false));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("v_flip").toString(), m_verticalInvBtn, false));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("record").toString(), m_recordBtn, false));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("rotate").toString(), m_rotateBtn, false));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("hide_img").toString(), m_hideImgBtn));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("next_frame").toString(), m_nextFrameBtn));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("prev_frame").toString(), m_prevFrameBtn));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("forward").toString(), m_forwardBtn));
    m_extensionShortcuts.append(SLV::createGlobalButtonShortcut(this, extShortcuts.value("backward").toString(), m_backwardBtn));

}

void ExtensionToolbar::enableButtons()
{
    // m_abloopBtn->setEnabled(true);
    m_backwardBtn->setEnabled(true);
    m_forwardBtn->setEnabled(true);
    m_recordBtn->setEnabled(true);
    m_rotateBtn->setEnabled(true);
    m_segmBtn->setEnabled(true);
    m_zoomBtn->setEnabled(true);
    m_compoRuleBtn->setEnabled(true);
    m_hideImgBtn->setEnabled(true);
    m_horizontalInvBtn->setEnabled(true);
    m_verticalInvBtn->setEnabled(true);
    m_nextFrameBtn->setEnabled(true);
    m_prevFrameBtn->setEnabled(true);
}

void ExtensionToolbar::disableButtons()
{
    // m_abloopBtn->setEnabled(false);
    m_backwardBtn->setEnabled(false);
    m_forwardBtn->setEnabled(false);
    m_recordBtn->setEnabled(false);
    m_rotateBtn->setEnabled(false);
    m_segmBtn->setEnabled(false);
    m_zoomBtn->setEnabled(false);
    m_compoRuleBtn->setEnabled(false);
    m_hideImgBtn->setEnabled(false);
    m_horizontalInvBtn->setEnabled(false);
    m_verticalInvBtn->setEnabled(false);
    m_nextFrameBtn->setEnabled(false);
    m_prevFrameBtn->setEnabled(false);
}

