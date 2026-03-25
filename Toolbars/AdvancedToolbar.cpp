#include "Toolbars/AdvancedToolbar.h"

#include "TextManager.h"
#include "ProjectManager.h"

#include "GenericDialog.h"

#include "Toolbars/ExtensionToolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"
#include "AdvancedToolbar.h"

#include "ProjectManager.h"
#include "TimeFormatter.h"

#include <QMessageBox>
#include <QPushButton>

AdvancedToolbar::AdvancedToolbar(QWidget *parent) : SimpleToolbar(parent)
{
    m_nextMediaBtn = new ToolbarButton(this, "next_white", TextManager::instance().get("tooltip_next_media"));
    m_prevMediaBtn = new ToolbarButton(this, "prev_white", TextManager::instance().get("tooltip_prev_media"));

    m_extensionBtn = new ToolbarToggleButton(this,
        false,
        "down_arrow_white",
        TextManager::instance().get("tooltip_minimize_toolbar"),
        "right_arrow_white",
        TextManager::instance().get("tooltip_expand_toolbar")
    );

    m_extensionToolbar = new ExtensionToolbar(this);


    
    connect(m_extensionBtn, &ToolbarToggleButton::stateActivated, m_extensionToolbar, [this](){
        // affiche l'extension et si le bouton de ségmentation est toujours on demande l'affichage de la segmentation
        m_extensionToolbar->show();
        m_extensionBtn->setButtonState(true);
        if(m_extensionToolbar->m_segmBtn->isChecked() && ProjectManager::instance().projet() != nullptr) emit enableSegmentationRequest();
    });
    connect(m_extensionBtn, &ToolbarToggleButton::stateDeactivated, m_extensionToolbar, [this](){
        // cache l'extension et cache le mode segmentation
        m_extensionToolbar->hide();
        m_extensionBtn->setButtonState(false);
        emit disableSegmentationRequest();
    });

    connect(m_prevMediaBtn, &ToolbarButton::clicked, this, &AdvancedToolbar::previousMediaRequested);
    connect(m_nextMediaBtn, &ToolbarButton::clicked, this, &AdvancedToolbar::nextMediaRequested);

    connect(m_extensionToolbar, &ExtensionToolbar::moveTimeBackwardRequested, this, &AdvancedToolbar::moveTimeBackwardRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::moveTimeForwardRequested, this, &AdvancedToolbar::moveTimeForwardRequested);

    connect(m_extensionToolbar, &ExtensionToolbar::rotateRequested, this, &AdvancedToolbar::rotateRequested);

    connect(m_extensionToolbar, &ExtensionToolbar::enableRecordRequested, this, &AdvancedToolbar::enableRecordRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::disableRecordRequested, this, &AdvancedToolbar::disableRecordRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::enableSegmentationRequested, this, [this](){
        emit AdvancedToolbar::enableSegmentationRequest();
        m_prevMediaBtn->setEnabled(false);
        m_nextMediaBtn->setEnabled(false);
    });

    connect(m_extensionToolbar, &ExtensionToolbar::disableSegmentationRequested, this, [this](){
        emit AdvancedToolbar::disableSegmentationRequest();
        m_prevMediaBtn->setEnabled(true);
        m_nextMediaBtn->setEnabled(true);
    });

    delete m_removePlayerBtn; // On ne veut pas de ce bouton dans cette toolbar
    m_removePlayerBtn = nullptr;

    setDefaultUI();
    disableButtons();
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

    QLabel* oldNameLabel = toolbar->nameLabel();
    if (oldTimeLabel && m_nameLabel) {
        m_nameLabel->setText(oldNameLabel->text());
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
    mainLayout->setContentsMargins(5,5,5,5);
    mainLayout->setSpacing(1);

    QHBoxLayout* timecodeLayout = new QHBoxLayout();
    timecodeLayout->addWidget(m_currentTimeLabel, 1, Qt::AlignLeft);
    timecodeLayout->addWidget(m_nameLabel, 1, Qt::AlignCenter);
    timecodeLayout->addWidget(m_durationLabel, 1, Qt::AlignRight);
    mainLayout->addLayout(timecodeLayout);

    mainLayout->addWidget(m_slider);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(1);
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_langBtn);
    buttonLayout->addSpacing(m_speedBtn->size().width());
    buttonLayout->addSpacing(m_speedBtn->size().width());


    buttonLayout->addStretch();

    buttonLayout->addWidget(m_speedBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_prevMediaBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_nextMediaBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_loopBtn);

    buttonLayout->addStretch();

    buttonLayout->addWidget(m_screenshotBtn);

    buttonLayout->addWidget(m_duplicatePlayerBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    buttonLayout->addWidget(m_extensionBtn);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(m_extensionToolbar);

}

void AdvancedToolbar::enableButtons()
{

    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    m_ejectBtn->setEnabled(true);
    // m_muteBtn->setEnabled(true);
    m_langBtn->setEnabled(true);
    m_loopBtn->setEnabled(true);
    m_duplicatePlayerBtn->setEnabled(true);
    // m_removePlayerBtn->setEnabled(true);
    // m_speedBtn->setEnabled(true);
    m_fullscreenBtn->setEnabled(true);
    m_extensionBtn->setEnabled(true);
    m_nextMediaBtn->setEnabled(true);
    m_prevMediaBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
    m_prevMediaBtn->setEnabled(true);
    m_nextMediaBtn->setEnabled(true);
    m_extensionToolbar->enableButtons();
}

void AdvancedToolbar::disableButtons()
{

    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_ejectBtn->setEnabled(false);
    // m_muteBtn->setEnabled(false);
    m_langBtn->setEnabled(false);
    m_loopBtn->setEnabled(false);
    m_duplicatePlayerBtn->setEnabled(false);
    // m_removePlayerBtn->setEnabled(false);
    // m_speedBtn->setEnabled(false);
    m_fullscreenBtn->setEnabled(false);
    m_extensionBtn->setEnabled(false);
    m_nextMediaBtn->setEnabled(false);
    m_prevMediaBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
    m_prevMediaBtn->setEnabled(false);
    m_nextMediaBtn->setEnabled(false);
    m_extensionToolbar->disableButtons();
}

void AdvancedToolbar::enableSlider(){
    m_slider->setDisabled(false);
    m_slider->setToolTip("");
}

void AdvancedToolbar::disableSlider(){
    m_slider->setDisabled(true);
    m_slider->setToolTip(TextManager::instance().get("tooltip_slider_disabled"));
}

void AdvancedToolbar::onSliderPressed() {
    SimpleToolbar::onSliderPressed(); 
}

void AdvancedToolbar::onSliderReleased() {
    SimpleToolbar::onSliderReleased(); 

    emit toolbarCursorPositionRequested(m_slider->value()); 
}

void AdvancedToolbar::onSliderMoved(int value) {
    SimpleToolbar::onSliderMoved(value); 

    emit toolbarCursorPositionRequested(value);

}

void AdvancedToolbar::duplicatePlayerAction()
{
    ProjectManager& projManager = ProjectManager::instance();
    if(projManager.needSave()){ 

        TextManager& txtManager = TextManager::instance();
        SLV::showGenericDialog(
            this, 
            txtManager.get("dialog_save_project_dialog_title"),
            txtManager.get("dialog_save_project_dialog_text"),
            [&projManager, this]() { 
                projManager.saveProject(false);
                emit this->duplicatePlayerRequested();
            },
            [&projManager, this]() { 
                emit this->duplicatePlayerRequested(); 
            }
        );
    }else {
        emit duplicatePlayerRequested();
    }
}

void AdvancedToolbar::ejectRequested(){
    ProjectManager& projManager = ProjectManager::instance();
    if(projManager.needSave()){ 

        TextManager& txtManager = TextManager::instance();

        SLV::showGenericDialog(
            this, 
            txtManager.get("dialog_save_project_dialog_title"),
            txtManager.get("dialog_save_project_dialog_text"),
            [&projManager]() { 
                projManager.saveProject(true); 
            },
            [&projManager]() { 
                projManager.discardAndEject();
            }
        );
        
    }else {
        projManager.discardAndEject();
    }
}
