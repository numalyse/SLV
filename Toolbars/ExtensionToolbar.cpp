#include "Toolbars/ExtensionToolbar.h"
#include "ProjectManager.h"

#include "TextManager.h"
#include <QHBoxLayout>
#include <SignalManager.h>
#include <qframe.h>

ExtensionToolbar::ExtensionToolbar(QWidget *parent) : QWidget(parent)
{

    m_zoomBtn = new ToolbarToggleButton(
        this, 
        false,
        "zoom_white",
        TextManager::instance().get("tooltip_zoom_on"),
        "zoom_white",
        TextManager::instance().get("tooltip_zoom_off")
    );

    m_hideImgBtn = new ToolbarToggleButton(
        this, 
        false,
        "hide_image_white",
        TextManager::instance().get("tooltip_hide_image"),
        "show_image_white",
        TextManager::instance().get("tooltip_show_image")
    );

    m_prevFrameBtn = new ToolbarButton(this, "prev_frame_white", TextManager::instance().get("tooltip_prev_frame"));
    m_nextFrameBtn = new ToolbarButton(this, "next_frame_white", TextManager::instance().get("tooltip_next_frame"));
    
    m_backwardBtn = new ToolbarButton(this, "backward_white", TextManager::instance().get("tooltip_backward"));
    m_forwardBtn = new ToolbarButton(this, "forward_white", TextManager::instance().get("tooltip_forward"));
    
    m_rotateBtn = new ToolbarButton(this, "rotate_white", TextManager::instance().get("tooltip_rotate"));

    m_recordBtn = new ToolbarToggleButton(
        this,
        false,
        "record_off_white",
        TextManager::instance().get("tooltip_record_off"),
        "record_on.png",
        TextManager::instance().get("tooltip_record_on")
    );
    
    m_segmBtn = new ToolbarToggleButton(
        this, 
        false,
        "timeline_on_white",
        TextManager::instance().get("tooltip_segmentation_on"),
        "timeline_off_white",
        TextManager::instance().get("tooltip_segmentation_off")
    );

    connect(m_segmBtn, &ToolbarToggleButton::stateActivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        if( ProjectManager::instance().projet() ){
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
    
    connect(m_backwardBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::moveTimeBackwardRequested);
    connect(m_forwardBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::moveTimeForwardRequested);

    connect(m_rotateBtn, &ToolbarButton::clicked, this, &ExtensionToolbar::rotateRequested);

    connect(m_recordBtn, &ToolbarToggleButton::stateActivated, this, &ExtensionToolbar::enableRecordRequested);
    connect(m_recordBtn, &ToolbarToggleButton::stateDeactivated, this, &ExtensionToolbar::disableRecordRequested);


    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, &ExtensionToolbar::disableSegmentationRequested);

    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        emit disableSegmentationRequested();
        m_segmBtn->setButtonState(false);
        emit SignalManager::instance().displayPlaylist();
    });

    m_compoRuleBtn = new ToolbarButton(this, "compo_rule_white", TextManager::instance().get("tooltip_composition"));

    QHBoxLayout* invFrameLayout = new QHBoxLayout();

    m_verticalInvBtn = new ToolbarButton(this, "invert_v_white", TextManager::instance().get("tooltip_flip_vertical"));
    m_horizontalInvBtn = new ToolbarButton(this, "invert_h_white", TextManager::instance().get("tooltip_flip_horizontal"));
    invFrameLayout->addWidget(m_verticalInvBtn);
    invFrameLayout->addWidget(m_horizontalInvBtn);
    m_invBtn = new ToolbarPopupButton(this, invFrameLayout, "invert_h_white", TextManager::instance().get("tooltip_flip_vertical"));

    setDefaultUI();
    disableButtons();
    hide();
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
