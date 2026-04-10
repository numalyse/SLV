#include "Toolbars/AdvancedToolbar.h"

#include "PrefManager.h"
#include "Project/ProjectManager.h"

#include "GenericDialog.h"

#include "Toolbars/ExtensionToolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"
#include "AdvancedToolbar.h"

#include "TimeFormatter.h"

#include "ShortcutHelper.h"

#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>

AdvancedToolbar::AdvancedToolbar(QWidget *parent) : SimpleToolbar(parent)
{
    m_nextMediaBtn = new ToolbarButton(this, "next_white", PrefManager::instance().getText("tooltip_next_media"));
    m_prevMediaBtn = new ToolbarButton(this, "prev_white", PrefManager::instance().getText("tooltip_prev_media"));

    m_extensionBtn = new ToolbarToggleButton(this,
        false,
        "down_arrow_white",
        PrefManager::instance().getText("tooltip_minimize_toolbar"),
        "right_arrow_white",
        PrefManager::instance().getText("tooltip_expand_toolbar")
    );

    m_extensionToolbar = new ExtensionToolbar(this);

    
    
    connect(m_extensionBtn, &ToolbarToggleButton::stateActivated, m_extensionToolbar, [this](){
        // affiche l'extension et si le bouton de ségmentation est toujours on demande l'affichage de la segmentation
        m_extensionToolbar->show();
        m_extensionBtn->setButtonState(true);
        if(m_extensionToolbar->m_segmBtn->isChecked() && ProjectManager::instance().projet() != nullptr) emit enableSegmentationRequest();
    });
    m_extensionBtn->setIconSize(QSize(13, 13));

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
    connect(m_extensionToolbar, &ExtensionToolbar::horizontalFlipRequested, this, &AdvancedToolbar::hFlipRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::verticalFlipRequested, this, &AdvancedToolbar::vFlipRequested);
    connect(this, &AdvancedToolbar::hFlipUiUpdate, m_extensionToolbar, &ExtensionToolbar::updateHFlipButtonUI);
    connect(this, &AdvancedToolbar::vFlipUiUpdate, m_extensionToolbar, &ExtensionToolbar::updateVFlipButtonUI);

    connect(m_extensionToolbar, &ExtensionToolbar::enableRecordRequested, this, &AdvancedToolbar::enableRecordRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::disableRecordRequested, this, &AdvancedToolbar::disableRecordRequested);

    connect(m_extensionToolbar, &ExtensionToolbar::prevFrameRequested, this, &AdvancedToolbar::prevFrameRequested);
    connect(m_extensionToolbar, &ExtensionToolbar::nextFrameRequested, this, &AdvancedToolbar::nextFrameRequested);
    
    connect(m_extensionToolbar, &ExtensionToolbar::setOverlayModeRequested, this, &AdvancedToolbar::setOverlayModeRequested);
    
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

    addShortcuts();
}

void AdvancedToolbar::addShortcuts(){
    auto& prefManager = PrefManager::instance();
    QJsonObject commonShortcuts = prefManager.getSubCategory("Shortcuts", "CommonToolbar");
    QJsonObject atShortcuts = prefManager.getSubCategory("Shortcuts", "AdvancedTB");

    addEnterFullscreenShortcut();

    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("play_pause").toString(), m_playPauseBtn));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("stop").toString(), m_stopBtn,  false));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("mute").toString(), m_muteBtn,  false));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, commonShortcuts.value("screenshot").toString(), m_screenshotBtn,  false));

    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, atShortcuts.value("next_media").toString(), m_nextMediaBtn, false));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, atShortcuts.value("prev_media").toString(), m_prevMediaBtn,  false));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, atShortcuts.value("loop").toString(), m_loopBtn));

    QShortcut* shortcutIncSpeed = new QShortcut(QKeySequence(atShortcuts.value("increase_speed").toString()), this);
    shortcutIncSpeed->setContext(Qt::ApplicationShortcut);
    connect(shortcutIncSpeed, &QShortcut::activated, this, &AdvancedToolbar::incrementSpeedSlider);
    m_advancedShortcuts.append(shortcutIncSpeed);

    QShortcut* shortcutDecSpeed = new QShortcut(QKeySequence(atShortcuts.value("decrease_speed").toString()), this);
    shortcutDecSpeed->setContext(Qt::ApplicationShortcut);
    connect(shortcutDecSpeed, &QShortcut::activated, this, &AdvancedToolbar::decrementSpeedSlider);
    m_advancedShortcuts.append(shortcutDecSpeed);

    QShortcut* shortcutResetSpeed = new QShortcut(QKeySequence(atShortcuts.value("base_speed").toString()), this);
    shortcutResetSpeed->setContext(Qt::ApplicationShortcut);
    connect(shortcutResetSpeed, &QShortcut::activated, this, &AdvancedToolbar::resetSpeedSlider);
    m_advancedShortcuts.append(shortcutResetSpeed);
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

    TimeEdit* oldTimeLE = toolbar->currentTimeLE();
    if (oldTimeLE && m_timeEdit) {
        m_timeEdit->setText(oldTimeLE->text());
        // TODO : set le temps du timecode modifiable
    }

    QPushButton* oldDurationBtn = toolbar->durationBtn();
    if (oldDurationBtn && m_durationBtn) {
        m_durationBtn->setText(oldDurationBtn->text());
        m_showRemainingTime = toolbar->showRemainingTime();

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
    if (oldNameLabel && m_nameLabel) {
        m_nameLabel->setText(oldNameLabel->text());
    }

    // TODO : voir pour copier les états des sliders dans muteBtn et speedBtn
    //addShortcuts();
}

AdvancedToolbar::~AdvancedToolbar()
{
    SLV::clearShortcuts(m_advancedShortcuts);
}

void AdvancedToolbar::setFullscreenUI()
{
    Toolbar::setFullscreenUI();
    if(m_extensionToolbar && m_extensionToolbar->getSegmBtn()->isChecked()){
        m_extensionToolbar->getSegmBtn()->click();
    }
    m_extensionToolbar->getSegmBtn()->setDisabled(true);
}

void AdvancedToolbar::setDefaultUI()
{
    Toolbar::setDefaultUI();
    
    m_extensionToolbar->getSegmBtn()->setDisabled(false);

    if ( !layout() ) {

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(5,5,5,5);
        mainLayout->setSpacing(1);

        QHBoxLayout* timecodeLayout = new QHBoxLayout();
        timecodeLayout->addWidget(m_timeEdit, 1, Qt::AlignLeft);
        timecodeLayout->addWidget(m_nameLabel, 1, Qt::AlignCenter);
        timecodeLayout->addWidget(m_durationBtn, 1, Qt::AlignRight);
        mainLayout->addLayout(timecodeLayout);

        mainLayout->addWidget(m_slider);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->setContentsMargins(0,0,0,0);
        buttonLayout->setSpacing(1);
        buttonLayout->addWidget(m_muteBtn);
        buttonLayout->addWidget(m_langBtn);
        buttonLayout->addSpacing(m_speedBtn->size().width());
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
        buttonLayout->addWidget(m_extractSequenceBtn);
        buttonLayout->addWidget(m_duplicatePlayerBtn);
        buttonLayout->addWidget(m_fullscreenBtn);
        buttonLayout->addWidget(m_extensionBtn);
        mainLayout->addLayout(buttonLayout);

        mainLayout->addWidget(m_extensionToolbar);
    }

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
    if(m_extractable)
        m_extractSequenceBtn->setEnabled(false);
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
    m_extractSequenceBtn->setEnabled(false);
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
    m_slider->setToolTip(PrefManager::instance().getText("tooltip_slider_disabled"));
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

        PrefManager& txtManager = PrefManager::instance();
        SLV::showGenericDialog(
            this, 
            txtManager.getText("dialog_save_project_dialog_title"),
            txtManager.getText("dialog_save_project_dialog_text"),
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

        PrefManager& txtManager = PrefManager::instance();

        SLV::showGenericDialog(
            this, 
            txtManager.getText("dialog_save_project_dialog_title"),
            txtManager.getText("dialog_save_project_dialog_text"),
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

void AdvancedToolbar::disableFullscreenRequested(){
    addEnterFullscreenShortcut();
    emit disableFullscreenRequest();
}

void AdvancedToolbar::incrementSpeedSlider(){
    if (m_speedSlider->value() < m_speedSlider->maximum()) {
        m_speedSlider->setValue(m_speedSlider->value() + 1);
    }
}

void AdvancedToolbar::decrementSpeedSlider(){
    if (m_speedSlider->value() > m_speedSlider->minimum()) {
        m_speedSlider->setValue(m_speedSlider->value() - 1);
    }
}

void AdvancedToolbar::resetSpeedSlider(){
    m_speedSlider->setValue(3);
}