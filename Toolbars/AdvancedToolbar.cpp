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
#include <QTimer>

AdvancedToolbar::AdvancedToolbar(QWidget *parent) : SimpleToolbar(parent)
{
    m_nextMediaBtn = new ToolbarButton(this, "next_white", PrefManager::instance().getText("tooltip_next_media") + "<br><i>("
    + PrefManager::instance().getText("tooltip_shortcut")
    + PrefManager::instance().getPref("Shortcuts", "AdvancedTB", "next_media") + ")</i>");
    m_prevMediaBtn = new ToolbarButton(this, "prev_white", PrefManager::instance().getText("tooltip_prev_media") + "<br><i>("
    + PrefManager::instance().getText("tooltip_shortcut")
    + PrefManager::instance().getPref("Shortcuts", "AdvancedTB", "prev_media") + ")</i>");
    m_nextMediaBtn->setEnabled(false);
    m_prevMediaBtn->setEnabled(false);

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
        if(m_extensionToolbar->m_segmBtn->isChecked() && ProjectManager::instance().projet() != nullptr)
            emit enableSegmentationRequest();
        else if (m_isFullscreen) {
            setFullscreenUI();
            setWindowOpacity(1.0); // force l'affichage de la barre 
        }
            
    });

    m_extensionBtn->setIconSize(QSize(13, 13));

    connect(m_extensionBtn, &ToolbarToggleButton::stateDeactivated, m_extensionToolbar, [this](){
        // cache l'extension et cache le mode segmentation
        m_extensionToolbar->hide();
        m_extensionBtn->setButtonState(false);
        emit disableSegmentationRequest();
        if (m_isFullscreen) {
            setFullscreenUI();
            setWindowOpacity(1.0); // force l'affichage de la barre 
        }
            
    });

    connect(m_prevMediaBtn, &ToolbarButton::clicked, this, [this](){
        // Disable buttons immediately to prevent rapid clicks from crashing
        m_prevMediaBtn->setEnabled(false);
        m_nextMediaBtn->setEnabled(false);
        
        // Delay signal emission + re-enable buttons after processing
        QTimer::singleShot(50, this, [this](){
            emit previousMediaRequested();
            // Re-enable after signal is processed
            QTimer::singleShot(200, this, [this](){
                m_prevMediaBtn->setEnabled(true);
                m_nextMediaBtn->setEnabled(true);
            });
        });
    });
    connect(m_nextMediaBtn, &ToolbarButton::clicked, this, [this](){
        // Disable buttons immediately to prevent rapid clicks from crashing
        m_prevMediaBtn->setEnabled(false);
        m_nextMediaBtn->setEnabled(false);
        
        // Delay signal emission + re-enable buttons after processing
        QTimer::singleShot(50, this, [this](){
            emit nextMediaRequested();
            // Re-enable after signal is processed
            QTimer::singleShot(200, this, [this](){
                m_prevMediaBtn->setEnabled(true);
                m_nextMediaBtn->setEnabled(true);
            });
        });
    });

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
    
    connect(m_extensionToolbar, &ExtensionToolbar::showBlackOpacityModeRequested, this, &AdvancedToolbar::showBlackOpacityModeRequested);

    connect(m_extensionToolbar, &ExtensionToolbar::setOverlayModeRequested, this, &AdvancedToolbar::setOverlayModeRequested);
    
    connect(m_extensionToolbar, &ExtensionToolbar::showDrawingModeRequested, this, [this](bool isEnabled) {
        m_maxFullscreenOpacity = isEnabled ? 0.3 : 1.0; 
        emit showDrawingModeRequested(isEnabled);
    });

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

    connect(&SignalManager::instance(), &SignalManager::activateMediaChangeBtn, this, [this](const bool state){
        m_prevMediaBtn->setEnabled(state);
        m_nextMediaBtn->setEnabled(state);
    });

    delete m_removePlayerBtn; // On ne veut pas de ce bouton dans cette toolbar
    m_removePlayerBtn = nullptr;

    delete layout(); // supprime le layout créer par le constructeur de SimpleToobar
    setDefaultUI();
    disableButtons();
    connect(&SignalManager::instance(), &SignalManager::playlistSizeResponse, this, [this](){
        m_prevMediaBtn->setEnabled(true);
        m_nextMediaBtn->setEnabled(true);
    });
    emit SignalManager::instance().requestPlaylistSize();
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

    // raccourcis "matériels"

    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaTogglePlayPause).toString(), m_playPauseBtn));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaPlay).toString(), m_playPauseBtn));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaPause).toString(), m_playPauseBtn));

    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaNext).toString(), m_nextMediaBtn, false));
    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_MediaPrevious).toString(), m_prevMediaBtn, false));

    m_advancedShortcuts.append(SLV::createGlobalButtonShortcut(this, QKeySequence(Qt::Key_VolumeMute).toString(), m_muteBtn, false));

}


/// @brief Constructeur qui va copier les états des boutons de le toolbar passé en paramète 
/// @param parent 
/// @param toolbar 
AdvancedToolbar::AdvancedToolbar(QWidget *parent, SimpleToolbar *toolbar) 
    : AdvancedToolbar(parent)
{
    if (!toolbar) return;

    m_media_fps = toolbar->mediaFps();
    m_media_duration = toolbar->mediaDuration();

    QSlider* oldSlider = toolbar->slider();
    if (oldSlider && m_slider) {
        m_slider->setRange(oldSlider->minimum(), oldSlider->maximum());
        m_slider->setValue(oldSlider->value());
    }

    TimeEdit* oldTimeLE = toolbar->currentTimeLE();
    if (oldTimeLE && m_timeEdit) {
        m_timeEdit->setText(oldTimeLE->text());
    }

    QPushButton* oldDurationBtn = toolbar->durationBtn();
    if (oldDurationBtn && m_durationBtn) {
        m_durationBtn->setChecked(oldDurationBtn->isChecked());
        updateDurationText(); 
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

    QSlider* oldVolumeSlider = toolbar->volumeSlider();
    if (oldVolumeSlider && m_volumeSlider) {
        m_volumeSlider->setValue(oldVolumeSlider->value());
    }
    QLabel* oldVolumeLabel = toolbar->volumeLabel();
    if (oldVolumeLabel && m_volumeLabel) {
        m_volumeLabel->setText(oldVolumeLabel->text());
    }

    QSlider* oldSpeedSlider = toolbar->speedSlider();
    if (oldSpeedSlider && m_speedSlider) {
        m_speedSlider->setValue(oldSpeedSlider->value());
    }
    QLabel* oldSpeedLabel = toolbar->speedLabel();
    if (oldSpeedLabel && m_speedLabel) {
        m_speedLabel->setText(oldSpeedLabel->text());
    }

    QLabel* oldNameLabel = toolbar->nameLabel();
    if (oldNameLabel && m_nameLabel) {
        m_nameLabel->setText(oldNameLabel->text());
    }

    QString oldStopTimeEdit = toolbar->customStopTimeEdit()->text();
    if (oldStopTimeEdit != "" && m_customStopTimeEdit) {
        m_customStopTimeEdit->setText(oldStopTimeEdit);
    }

    bool oldStopCheckbox = toolbar->customStopCheckbox()->isChecked();
    if (m_customStopCheckbox) {
        m_customStopCheckbox->setChecked(oldStopCheckbox);
    }
}

AdvancedToolbar::~AdvancedToolbar()
{
    SLV::clearShortcuts(m_advancedShortcuts);
}

void AdvancedToolbar::setFullscreenUI(int bottomMargin)
{

    m_duplicatePlayerBtn->setDisabled(true);
    auto* segmentationBtn = m_extensionToolbar->getSegmBtn();

    if(m_extensionToolbar && segmentationBtn->isChecked()){ // si la timeline est affiché on simule le click pour la cacher
        segmentationBtn->click();
    }
    segmentationBtn->setDisabled(true);

    Toolbar::setFullscreenUI(bottomMargin);
}


void AdvancedToolbar::setDefaultUI()
{
    Toolbar::setDefaultUI();

    m_duplicatePlayerBtn->setDisabled(false);
    m_extensionToolbar->getSegmBtn()->setDisabled(false);

    if( !layout() ) {

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(5,5,5,5);
        mainLayout->setSpacing(1);

        m_nameLabel->hide();
        m_stopBtn->hide();

        QHBoxLayout* timecodeLayout = new QHBoxLayout();
        timecodeLayout->addWidget(m_timeEdit);
        timecodeLayout->addWidget(m_slider, 1);
        timecodeLayout->addWidget(m_durationBtn);
        mainLayout->addLayout(timecodeLayout);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->setContentsMargins(0,0,0,0);
        buttonLayout->setSpacing(1);
        buttonLayout->addWidget(m_muteBtn);
        buttonLayout->addWidget(m_langBtn);
        buttonLayout->addWidget(m_mediaInfoBtn);
        buttonLayout->addSpacing(m_speedBtn->width()+1);
        buttonLayout->addSpacing(m_speedBtn->width()+1);
        buttonLayout->addSpacing(m_speedBtn->width()+1);
        buttonLayout->addSpacing(m_zoomIndicator->width()+1);

        buttonLayout->addStretch();

        buttonLayout->addWidget(m_speedBtn);
        buttonLayout->addWidget(m_prevMediaBtn);
        buttonLayout->addWidget(m_customStopBtn);
        //buttonLayout->addWidget(m_stopBtn); MODIF
        buttonLayout->addWidget(m_playPauseBtn);
        buttonLayout->addWidget(m_ejectBtn);
        buttonLayout->addWidget(m_nextMediaBtn);
        buttonLayout->addWidget(m_loopBtn);

        buttonLayout->addStretch();

        buttonLayout->addWidget(m_zoomIndicator);
        buttonLayout->addWidget(m_zoomBtn);
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
    SimpleToolbar::enableButtons();

    m_extensionBtn->setEnabled(true);
    // m_nextMediaBtn->setEnabled(true);
    // m_prevMediaBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
    m_mediaInfoBtn->setEnabled(true);
    m_zoomBtn->setEnabled(true);
    m_extensionToolbar->enableButtons();
}

void AdvancedToolbar::disableButtons()
{
    SimpleToolbar::disableButtons();

    m_extensionBtn->setEnabled(false);
    // m_nextMediaBtn->setEnabled(false);
    // m_prevMediaBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
    m_mediaInfoBtn->setEnabled(false);
    m_zoomBtn->setEnabled(false);
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

void AdvancedToolbar::updateRotationTooltip(const int rotationIndex)
{
    QString tooltipIndicator;
    switch(rotationIndex){
    case 0:
        tooltipIndicator = "";
        break;
    case 1:
        tooltipIndicator = "\n" + PrefManager::instance().getText("tooltip_current_rotation") + "270°";
        break;
    case 2:
        tooltipIndicator = "\n" + PrefManager::instance().getText("tooltip_current_rotation") + "180°";
        break;
    case 3:
        tooltipIndicator = "\n" + PrefManager::instance().getText("tooltip_current_rotation") + "90°";
    }

    m_extensionToolbar->m_rotateBtn->setToolTip(PrefManager::instance().getText("tooltip_rotate") + tooltipIndicator);
}

void AdvancedToolbar::updateFlipTooltip(const bool hFlip, const bool vFlip)
{
    QString tooltipIndicator = PrefManager::instance().getText("tooltip_flip");
    if(hFlip)
        tooltipIndicator += "\n" + PrefManager::instance().getText("tooltip_h_flipped");
    if(vFlip)
        tooltipIndicator += "\n" + PrefManager::instance().getText("tooltip_v_flipped");

    m_extensionToolbar->m_invBtn->setToolTip(tooltipIndicator);
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
