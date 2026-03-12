#include "Toolbars/ExtensionToolbar.h"
#include "ProjectManager.h"

#include "TextManager.h"
#include <QHBoxLayout>
#include <SignalManager.h>

ExtensionToolbar::ExtensionToolbar(QWidget *parent) : QWidget(parent)
{

    m_zoomBtn = new ToolbarToggleButton(
        this, 
        false,
        "zoom_on.png", 
        TextManager::instance().get("tooltip_zoom_on"),
        "zoom_on.png",
        TextManager::instance().get("tooltip_zoom_off")
    );

    m_hideImgBtn = new ToolbarToggleButton(
        this, 
        false,
        "hide_image.png", 
        TextManager::instance().get("tooltip_hide_image"),
        "show_image.png", 
        TextManager::instance().get("tooltip_show_image")
    );

    m_prevFrameBtn = new ToolbarButton(this, "prev_frame.png", TextManager::instance().get("tooltip_prev_frame"));
    m_nextFrameBtn = new ToolbarButton(this, "next_frame.png", TextManager::instance().get("tooltip_next_frame"));
    
    m_backwardBtn = new ToolbarButton(this, "backward.png", TextManager::instance().get("tooltip_backward"));
    m_forwardBtn = new ToolbarButton(this, "forward.png", TextManager::instance().get("tooltip_forward"));
    
    m_rotateBtn = new ToolbarButton(this, "rotate.png", TextManager::instance().get("tooltip_rotate"));
    
    m_segmBtn = new ToolbarToggleButton(
        this, 
        false,
        "segmentation_on.png", 
        TextManager::instance().get("tooltip_segmentation_on"),
        "segmentation_off.png", 
        TextManager::instance().get("tooltip_segmentation_off")
    );

    connect(m_segmBtn, &ToolbarToggleButton::stateActivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        if(ProjectManager::instance().projet() != nullptr){
            m_segmBtn->setButtonState(true);
            emit enableSegmentationRequested();
            emit SignalManager::instance().extensionToolbarDisplayShotDetail();
        }
    });

    connect(&ProjectManager::instance(), &ProjectManager::projectDeleted, this, [this] { // quand le projet est détruit, on force le button segmentation en false   
            m_segmBtn->setButtonState(false); 
    });

    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, &ExtensionToolbar::disableSegmentationRequested);

    connect(m_segmBtn, &ToolbarToggleButton::stateDeactivated, this, [this] { // vérifie qu'il y a bien un projet avant d'afficher la timeline
        emit disableSegmentationRequested();
        m_segmBtn->setButtonState(false);
        emit SignalManager::instance().displayPlaylist();
    });

    m_compoRuleBtn = new ToolbarButton(this, "composition.png", TextManager::instance().get("tooltip_composition"));
    
    m_verticalInvBtn = new ToolbarButton(this, "flip_vertical.png", TextManager::instance().get("tooltip_flip_vertical"));
    m_horizontalInvBtn = new ToolbarButton(this, "flip_horizontal.png", TextManager::instance().get("tooltip_flip_horizontal"));

    setDefaultUI();
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

    mainLayout->addWidget(m_zoomBtn);
    mainLayout->addWidget(m_hideImgBtn);
    
    mainLayout->addWidget(m_backwardBtn);
    mainLayout->addWidget(m_prevFrameBtn);
    mainLayout->addWidget(m_nextFrameBtn);
    mainLayout->addWidget(m_forwardBtn);
    
    mainLayout->addWidget(m_rotateBtn);
    mainLayout->addWidget(m_verticalInvBtn);
    mainLayout->addWidget(m_horizontalInvBtn);
    
    mainLayout->addWidget(m_segmBtn);
    mainLayout->addWidget(m_compoRuleBtn);

}
