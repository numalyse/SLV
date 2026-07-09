#include "PlayerWidget.h"
#include "Toolbars/SimpleToolbar.h"
#include "Project/ProjectManager.h"
#include "SignalManager.h"
#include "CompositionWidget.h"
#include "BlackOpacityWidget.h"
#include "FileFormatManager.h"
#include "GenericDialog.h"

#ifdef __APPLE__
#include "MacWindowHelper.h"
#endif

#include <QDebug>
#include <QApplication>
#include <QResizeEvent>
#include <QShowEvent>
#include <QFile>
#include <QKeyEvent>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QStackedLayout>

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    //setMinimumSize(640, 360);
    //resize(800, 450);

    // setAttribute(Qt::WA_NativeWindow);
    // setAttribute(Qt::WA_DontCreateNativeAncestors);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAcceptDrops(true);

    // ===== Toolbar ===== //
    m_toolBar = new SimpleToolbar(this);
    //m_toolBar->setDefaultUI();

    connect(m_toolBar, &SimpleToolbar::removePlayerRequest, this, [this]() {
        emit removePlayerRequest(this);
    });

    connect(m_toolBar, &SimpleToolbar::duplicatePlayerRequested, this, [this](){
        resetLayerWidgets();
        emit duplicatePlayerRequest(this);
    });

    m_mediaWidget = new MediaWidget(this);

    connect(m_toolBar, &Toolbar::playRequest, this, &PlayerWidget::play);
    connect(m_toolBar, &Toolbar::pauseRequest, this, &PlayerWidget::pause);
    connect(m_toolBar, &Toolbar::stopRequest, this, &PlayerWidget::stop);
    connect(m_toolBar, &Toolbar::ejectRequest, this, &PlayerWidget::eject);
    connect(m_toolBar, &Toolbar::enableFullscreenRequest, this, &PlayerWidget::enablePlayerFullscreen);
    connect(m_toolBar, &Toolbar::disableFullscreenRequest, this, &PlayerWidget::disablePlayerFullscreen);
    connect(m_toolBar, &SimpleToolbar::enableMuteRequest, this, &PlayerWidget::mute);
    connect(m_toolBar, &SimpleToolbar::disableMuteRequest, this, &PlayerWidget::unmute);
    connect(m_toolBar, &SimpleToolbar::volumeChanged, m_mediaWidget, &MediaWidget::setVolume);
    connect(m_toolBar, &SimpleToolbar::speedChanged, m_mediaWidget, &MediaWidget::setSpeed);
    connect(m_toolBar, &Toolbar::screenshotRequest, m_mediaWidget, &MediaWidget::takeScreenshot);
    connect(m_toolBar, &SimpleToolbar::setPositionRequested, this, &PlayerWidget::setTime);
    connect(m_toolBar, &SimpleToolbar::enableLoopModeRequest, this, &PlayerWidget::enableLoopMode);
    connect(m_toolBar, &SimpleToolbar::disableLoopModeRequest, this, &PlayerWidget::disableLoopMode);
    connect(m_toolBar, &SimpleToolbar::extractSequenceRequest, this, &PlayerWidget::openSequenceExtractionDialog);
    connect(m_toolBar, &SimpleToolbar::mediaInformationRequest, m_mediaWidget, &MediaWidget::openMediaInfoDialog);
    connect(m_toolBar, &SimpleToolbar::enableZoomMode, this, &PlayerWidget::enableZoomMode);
    connect(m_toolBar, &SimpleToolbar::disableZoomMode, this, &PlayerWidget::disableZoomMode);
    connect(m_toolBar, &SimpleToolbar::subtitlesFileDialogRequested, this, &PlayerWidget::openSubtitlesFileDialog);

    connect(this, &PlayerWidget::playUiUpdateRequested, m_toolBar, &SimpleToolbar::playUiUpdate);
    connect(this, &PlayerWidget::pauseUiUpdateRequested, m_toolBar, &SimpleToolbar::pauseUiUpdate);
    connect(this, &PlayerWidget::muteUiUpdateRequested, m_toolBar, &SimpleToolbar::muteUiUpdate);
    connect(this, &PlayerWidget::unmuteUiUpdateRequested, m_toolBar, &SimpleToolbar::unmuteUiUpdate);
    connect(this, &PlayerWidget::ejectUiUpdateRequested, m_toolBar, &SimpleToolbar::ejectUiUpdate);
    connect(this, &PlayerWidget::stopUiUpdateRequested, m_toolBar, &SimpleToolbar::stopUiUpdate);
    connect(this, &PlayerWidget::enableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::enableLoopUiUpdate);
    connect(this, &PlayerWidget::disableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::disableLoopUiUpdate);
    connect(this, &PlayerWidget::nameUiUpdateRequest, m_toolBar, &SimpleToolbar::nameUiUpdate);

    connect(m_mediaWidget, &MediaWidget::pauseUiUpdateRequested, this, &PlayerWidget::pauseUiUpdateRequested);
    connect(m_mediaWidget, &MediaWidget::volumeChanged, m_toolBar, &SimpleToolbar::volumeUiUpdate);
    connect(m_mediaWidget, &MediaWidget::speedChanged, m_toolBar, &SimpleToolbar::speedUiUpdate);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerLoaded, this, &PlayerWidget::enableButtons);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerEjected, this, &PlayerWidget::disableButtons);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerLoaded, this, &PlayerWidget::mediaPlayerLoaded);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerEjected, this, &PlayerWidget::mediaPlayerEjectedHandler);
    connect(m_mediaWidget, &MediaWidget::togglePlayPauseRequested, this, &PlayerWidget::togglePlayPause);
    connect(m_mediaWidget, &MediaWidget::zoomValueUpdated, m_toolBar, &SimpleToolbar::setZoomIndicatorText);
    connect(m_mediaWidget, &MediaWidget::hideAudioLogo, this, [this](){
        m_audioLogoWidget->setDisplay(false);
        m_dragDropLogoWidget->setDisplay(false);
    });
    connect(m_mediaWidget, &MediaWidget::showAudioLogo, this, [this](){
        m_audioLogoWidget->setDisplay(true);
        updateSingleLogoGeom(m_audioLogoWidget, true);
        m_dragDropLogoWidget->setDisplay(false);
    });

    //connect(m_mediaWidget, &MediaWidget::subtitleTrackAdded, m_toolBar, &SimpleToolbar::subtitleTrackAdd);
    connect(m_mediaWidget, &MediaWidget::subtitleTrackAdded, this, [this](int trackId, const QString &label){
        auto success = m_toolBar->subtitleTrackAdd(trackId, label);
        if (success) QMessageBox::information(nullptr, "", PrefManager::instance().getText("added_subtitles"));
        else QMessageBox::warning(nullptr, "", PrefManager::instance().getText("conflict_subtitles"));
    });

    connect(m_mediaWidget, &MediaWidget::playbackStarted, this, [this](){
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    });

    connect(m_mediaWidget, &MediaWidget::playbackPaused, this, [this](){
        emit pauseUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    });

    connect(this, &PlayerWidget::mediaDropped, &SignalManager::instance(), &SignalManager::playerWidgetMediaDropped);

    QWidget* containerWidget = new QWidget(this);
    QStackedLayout* stack = new QStackedLayout(containerWidget);
    stack->setContentsMargins(0,0,0,0);
    stack->setStackingMode(QStackedLayout::StackAll);
    stack->addWidget(m_mediaWidget);

    m_dragDropLogoWidget = new MediaLogoWidget(containerWidget, ":/icons/drag_drop_grey", 100);
    m_dragDropLogoWidget->setDisplay(true);

    m_audioLogoWidget = new MediaLogoWidget(containerWidget, ":/icons/music_note_grey", 200);
    m_audioLogoWidget->setDisplay(false);
    //stack->addWidget(m_audioLogoWidget);

    m_compositionWidget = new CompositionWidget(containerWidget);
    //stack->addWidget(m_compositionWidget);

    m_blackOpacityWidget = new BlackOpacityWidget(containerWidget);
    //stack->addWidget(m_blackOpacityWidget);

    m_drawingWidget = new DrawingWidget(containerWidget);
    //stack->addWidget(m_drawingWidget);

#ifdef __APPLE__
    // not attachable here since this has no window yet
    // and attach would show the widget
    // on widget show => attach / hide => detach
    m_compositionWidget->installEventFilter(this);
    m_blackOpacityWidget->installEventFilter(this);
    m_drawingWidget->installEventFilter(this);
#endif

    //m_compositionWidget->setOverlayMode(CompositionWidget::GoldenRatio);
    //m_compositionWidget->raise();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(containerWidget);
    layout->addWidget(m_toolBar);

    connect(m_mediaWidget, &MediaWidget::updateSliderRangeRequested, this, &PlayerWidget::updateSliderRangeRequest);
    connect(m_mediaWidget, &MediaWidget::vlcTimeChanged, this, &PlayerWidget::vlcTimeChanged);
    connect(m_mediaWidget, &MediaWidget::updateFpsRequested, this, &PlayerWidget::updateFpsRequest);
    connect(m_mediaWidget, &MediaWidget::nameUiUpdateRequested, this, &PlayerWidget::nameUiUpdateRequest);

    connect(this, &PlayerWidget::updateSliderRangeRequest, m_toolBar, &SimpleToolbar::updateSliderRange);
    connect(this, &PlayerWidget::vlcTimeChanged, m_toolBar, &SimpleToolbar::updateSliderValue);
    connect(this, &PlayerWidget::updateFpsRequested, m_toolBar, &SimpleToolbar::updateFps);

    connect(&SignalManager::instance(), &SignalManager::timelineSetPosition, this, &PlayerWidget::setTime);

    connect(m_mediaWidget, &MediaWidget::mediaRectChanged, this, &PlayerWidget::onMediaRectChanged);
    connect(this, &PlayerWidget::mediaRectChanged, m_blackOpacityWidget, &BlackOpacityWidget::onMediaRectChanged);
    connect(this, &PlayerWidget::mediaRectChanged, m_compositionWidget, &CompositionWidget::onMediaRectChanged);
    connect(this, &PlayerWidget::mediaRectChanged, m_drawingWidget, &DrawingWidget::onMediaRectChanged);
    connect(this, &PlayerWidget::mediaRectChanged, m_audioLogoWidget, &MediaLogoWidget::onMediaRectChanged);
    connect(&SignalManager::instance(), &SignalManager::windowMovedOrResized, this, &PlayerWidget::widgetSizeChange);

    connect(m_mediaWidget, &MediaWidget::updateAudioTracksRequested, m_toolBar, &SimpleToolbar::updateAudioTracks);
    connect(m_mediaWidget, &MediaWidget::updateSubtitlesTracksRequested, m_toolBar, &SimpleToolbar::updateSubtitlesTracks);

    connect(m_mediaWidget, &MediaWidget::setAudioTrackRequested, m_toolBar, &SimpleToolbar::setAudioTrackDefault);
    connect(m_mediaWidget, &MediaWidget::setSubtitlesTrackRequested, m_toolBar, &SimpleToolbar::setSubtitlesTrackDefault);
    connect(m_toolBar, &SimpleToolbar::setAudioTrackRequested, m_mediaWidget, &MediaWidget::setAudioTrack);
    connect(m_toolBar, &SimpleToolbar::setSubtitlesTrackRequested, m_mediaWidget, &MediaWidget::setSubtitleTrack);

    restoreOverlayStackOrder();
}

PlayerWidget::~PlayerWidget()
{
#ifdef __APPLE__
    MacWindowHelper::detachFromParentWindow(m_compositionWidget);
    MacWindowHelper::detachFromParentWindow(m_blackOpacityWidget);
    MacWindowHelper::detachFromParentWindow(m_drawingWidget);
#endif
     
}


void PlayerWidget::setActive(bool active)
{
    setStyleSheet(active
                      ? "border: 2px solid #2a82da;"
                      : "border: none;");
}

bool PlayerWidget::setMediaFromPath(const QString& filePath)
{
    if (m_mediaWidget->setMediaFromPath(filePath)){
        m_muted = false;
        emit playUiUpdateRequested();
        emit unmuteUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
        emit checkPlayersMuteStatusRequested();
        return true;
    }
    return false;
}

QString PlayerWidget::getMediaPath()
{
    if (mediaWidget() && mediaWidget()->media()) {
        return mediaWidget()->media()->filePath();
    }
    return QString();
}

int PlayerWidget::getCurrentTime()
{
    if(mediaWidget() && mediaWidget()->media()){
        return mediaWidget()->getCurrentTime();
    }
    return -1;
}

double PlayerWidget::getSar()
{
    if(!m_mediaWidget || !m_mediaWidget->media()) return 1.0;
    else return m_mediaWidget->media()->sar();
}

void PlayerWidget::enablePlayerFullscreen()
{
    emit enablePlayerFullscreenRequested(this);
    m_toolBar->enableFullscreenUiUpdate();
}

void PlayerWidget::disablePlayerFullscreen()
{
    emit disablePlayerFullscreenRequested(this);
    m_toolBar->disableFullscreenUiUpdate();
}

// slots

/// @brief Play la video, si pas de media dans le player : créer un QFileDialog pour choisir un fichier à charger.
void PlayerWidget::play()
{
    if (m_mediaWidget->play()){
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }else {
        auto& prefManager = PrefManager::instance();
        QString file_path = QFileDialog::getOpenFileName(
            this,
            prefManager.getText("dialog_open_file"),
            prefManager.getPref("Paths", "lp_open_media"),
            FileFormatManager::instance().getOpenFileDialogFilters()
        );
        if(file_path != ""){
            setMediaFromPath(file_path);
            QFileInfo fileInfo (file_path);
            prefManager.setPref("Paths", "lp_open_media", fileInfo.absolutePath());
        }
        else
            emit m_toolBar->selectFilePlayCanceled();
    }
}

/// @brief Play la video connecté à une advanced toolbar, créer un projet en plus de l'action de base
void PlayerWidget::playFromAdvanced()
{
    if (m_mediaWidget->play()){
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }else {

        auto& prefManager = PrefManager::instance();
        QString file_path = QFileDialog::getOpenFileName(
            this,
            prefManager.getText("dialog_open_file"),
            prefManager.getPref("Paths", "lp_open_media"),
            FileFormatManager::instance().getOpenFileDialogFilters()
        );

        if(file_path != ""){
            if (setMediaFromPath(file_path)){
                ProjectManager::instance().requestProjectCreation({file_path});
                QFileInfo fileInfo (file_path);
                prefManager.setPref("Paths", "lp_open_media", fileInfo.absolutePath());
            }
        }
        else
            emit m_toolBar->selectFilePlayCanceled();
    }
}

void PlayerWidget::pause()
{
    m_mediaWidget->pause();
}

void PlayerWidget::togglePlayPause(bool isPlaying)
{
    if(isPlaying) pause();
    else if(!m_toolBar->isVisible()) playFromAdvanced();
    else play();
}

void PlayerWidget::stop()
{
    m_mediaWidget->stop();
}

void PlayerWidget::eject()
{
    resetLayerWidgets();
    m_mediaWidget->eject();
    // disconnect(this, nullptr, nullptr, nullptr);
}

void PlayerWidget::mute()
{
    if(m_mediaWidget->mute()){
        m_muted = true;
        emit muteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }

}

void PlayerWidget::unmute()
{
    if(m_mediaWidget->unmute()){
        m_muted = false;
        emit unmuteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }
}

void PlayerWidget::setVolume(const int &vol)
{
    m_mediaWidget->setVolume(vol);
}

void PlayerWidget::setSpeed(const unsigned int &speed)
{
    m_mediaWidget->setSpeed(speed);
}
void PlayerWidget::setTime(int64_t time){
    m_mediaWidget->setTime(time);
}

void PlayerWidget::moveTimeBackward(){
    m_mediaWidget->moveTimeBackward();
}

void PlayerWidget::moveTimeForward(){
    m_mediaWidget->moveTimeForward();
}

void PlayerWidget::updateFpsRequest(double newFps){
    m_media_fps = newFps;
    emit updateFpsRequested(newFps);
}

void PlayerWidget::takeScreenshot()
{
    m_mediaWidget->takeScreenshot();
}

void PlayerWidget::enableLoopMode()
{
    m_mediaWidget->enableLoopMode();
    emit enableLoopUiUpdateRequested();
}

void PlayerWidget::disableLoopMode()
{
    m_mediaWidget->disableLoopMode();
    emit disableLoopUiUpdateRequested();
}

void PlayerWidget::enableZoomMode()
{
    m_mediaWidget->enableZoomMode();
    m_toolBar->zoomBtn()->setButtonState(true);
    emit enableZoomUiUpdateRequested();
}

void PlayerWidget::disableZoomMode()
{
    m_mediaWidget->disableZoomMode();
    m_toolBar->zoomBtn()->setButtonState(false);
    emit disableZoomUiUpdateRequested();
}

void PlayerWidget::startRecord()
{
    m_mediaWidget->startRecord();
    emit SignalManager::instance().recordButtonUiUpdate();
}

void PlayerWidget::endRecord()
{
    m_mediaWidget->endRecord();
    emit SignalManager::instance().recordButtonUiUpdate();
}

void PlayerWidget::rotate()
{
    m_mediaWidget->rotate();
}

void PlayerWidget::setBlackOpacityMode(bool isShown, double opacity){
    m_blackOpacityWidget->setBlackOpacityMode(isShown, opacity);
    updateSingleOverlayGeom(m_blackOpacityWidget, isShown);
}

void PlayerWidget::showDrawingMode(bool isEnabled){
    m_drawingWidget->showDrawingMode(isEnabled);
    updateSingleOverlayGeom(m_drawingWidget, isEnabled);

}

void PlayerWidget::setOverlayMode(bool showOverlay, OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked){
    overlayMode = (showOverlay) ? overlayMode : OverlayMode::None;
    m_compositionWidget->setOverlayMode(overlayMode, vFlipChecked, hFlipChecked);
    updateSingleOverlayGeom(m_compositionWidget, showOverlay);
}

void PlayerWidget::openSequenceExtractionDialog()
{
    if(m_mediaWidget->media()->type() != MediaType::Video){
        QMessageBox::warning(this, "", PrefManager::instance().getText("messagebox_not_a_video"));
        return;
    }
    pause();
    ExtractSequenceWidget* sequenceExtractor = new ExtractSequenceWidget(*m_mediaWidget->media(), this, m_mediaWidget->getCurrentTime());

    sequenceExtractor->open();

}

void PlayerWidget::onMediaRectChanged(const QRect &rect)
{
    m_mediaRect = rect;
    emit mediaRectChanged(m_mediaRect);
    qDebug() << "PlayerWidget m_mediaRect : " << m_mediaRect;
}

void PlayerWidget::widgetSizeChange()
{

    if (!m_blackOpacityWidget || !m_compositionWidget || !m_drawingWidget || !m_mediaWidget ||!m_audioLogoWidget)
        return; 

    if(!isVisible()) return;

    QRect globalRect(m_mediaWidget->mapToGlobal(QPoint(0, 0)), m_mediaWidget->size());
    QRect localRect(m_mediaWidget->mapTo(this, QPoint(0, 0)), m_mediaWidget->size());

    auto updateGeometryIfVisible = [](QWidget* widget, const QRect& rect) {
        if (widget->isVisible()) {
            widget->setGeometry(rect);
        }
    };
    
    updateGeometryIfVisible(m_dragDropLogoWidget, localRect);
    updateGeometryIfVisible(m_audioLogoWidget, localRect);

    updateGeometryIfVisible(m_compositionWidget, globalRect);
    updateGeometryIfVisible(m_blackOpacityWidget, globalRect);
    updateGeometryIfVisible(m_drawingWidget, globalRect);
}

bool PlayerWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Hide:
        m_dragDropLogoWidget->setDisplay(false);
        break;
    case QEvent::Show:{
        if(getMediaPath() == "")  {
            m_dragDropLogoWidget->setDisplay(true);
            updateSingleLogoGeom(m_dragDropLogoWidget, true);
        }
        else m_dragDropLogoWidget->setDisplay(false);
        m_blackOpacityWidget->show();
        m_compositionWidget->show();
        restoreOverlayStackOrder();
        QTimer::singleShot(0, this, SLOT(widgetSizeChange())); 
        break;
    }
    case QEvent::WindowActivate:
    case QEvent::Resize:
    case QEvent::Move:
        m_toolBar->updateFullscreenPosition();
        widgetSizeChange();
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

bool PlayerWidget::eventFilter(QObject* watched, QEvent* event)
{
#ifdef __APPLE__
    // attach watched overlay as child to prevent delay
    if (watched == m_compositionWidget || watched == m_blackOpacityWidget || watched == m_drawingWidget) {
        QWidget* overlay = static_cast<QWidget*>(watched);
        if (event->type() == QEvent::Show)
            MacWindowHelper::attachAsChildWindow(overlay, this);
        else if (event->type() == QEvent::Hide)
            MacWindowHelper::detachFromParentWindow(overlay);
    }
#endif
    return QWidget::eventFilter(watched, event);
}

void PlayerWidget::enableButtons()
{
    m_toolBar->enableButtons();
}

void PlayerWidget::disableButtons()
{
    m_toolBar->disableButtons();
}

void PlayerWidget::mediaPlayerEjectedHandler()
{
    m_audioLogoWidget->setDisplay(false);
    if(isVisible()){
        m_dragDropLogoWidget->setDisplay(true);
        updateSingleLogoGeom(m_dragDropLogoWidget, true);
    }else {
        m_dragDropLogoWidget->setDisplay(false);
    }    
    emit ejectUiUpdateRequested();
    emit checkPlayersPlayStatusRequested();
    emit SignalManager::instance().displayPlaylist();
    emit mediaPlayerEjected();

    // Charger le fichier en attente après eject
    if (!m_pendingFilePath.isEmpty()) {
        setMediaFromPath(m_pendingFilePath);
        if(!m_toolBar->isVisible()){
            ProjectManager::instance().requestProjectCreation({m_pendingFilePath});
            QFileInfo fileInfo (m_pendingFilePath);
            PrefManager::instance().setPref("Paths", "lp_open_media", fileInfo.absolutePath());
        }
        m_pendingFilePath.clear();
    }
}

void PlayerWidget::resetLayerWidgets()
{
    m_compositionWidget->setOverlayMode(OverlayMode::None, false, false);
    m_drawingWidget->showDrawingMode(false);
    m_blackOpacityWidget->setBlackOpacityMode(false, 0);
    restoreOverlayStackOrder();
}

void PlayerWidget::openSubtitlesFileDialog()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        PrefManager::instance().getText("subtitles_path_selection"),
        getMediaPath(),
        FileFormatManager::instance().getFormats("subtitles")
    );

    if(filePath.isEmpty()) return;

    m_mediaWidget->addSubtitles(filePath);
}

void PlayerWidget::dragEnterEvent(QDragEnterEvent *event){
    if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
}

void PlayerWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QStringList filePaths;

        for (const QUrl &url : urlList) {
            QString filePath = url.toLocalFile();
            QFileInfo info(filePath);
            qDebug() << "Fichier droppé :" << filePath;
            if(FileFormatManager::instance().isFormatAccepted(info.suffix())) filePaths.append(filePath);
            else if(info.isDir()){
                ProjectManager::instance().openProjectFromPath(filePath);
                event->acceptProposedAction();
                return;
            }
            if (filePaths.size() >= 4) break;
        }

        bool fileNotSupported = filePaths.size() < event->mimeData()->urls().size();
        if(fileNotSupported){
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::StandardButton::Ok);
            msg.setInformativeText(PrefManager::instance().getText("messagebox_format_not_accepted"));
            msg.setIcon(QMessageBox::Information);
            msg.exec();
        }

        if(filePaths.empty()){
            event->ignore();
            return;
        }
        else if (filePaths.size() == 1) {
            if(m_mediaWidget->media()){
                if(ProjectManager::instance().needSave()){

                    PrefManager& txtManager = PrefManager::instance();
                    bool canceled = false;
                    SLV::showGenericDialog(
                        this,
                        txtManager.getText("dialog_save_project_dialog_title"),
                        txtManager.getText("dialog_save_project_dialog_text"),
                        []() {
                            ProjectManager::instance().saveProject(false);
                        },
                        [](){},
                        [&canceled](){ canceled = true; }
                    );
                    if(canceled) return;
                }
                m_pendingFilePath = filePaths.first();
                eject();
            }
            else{
                if (setMediaFromPath(filePaths.first()) && !m_toolBar->isVisible()){
                    ProjectManager::instance().requestProjectCreation({filePaths.first()});
                    QFileInfo fileInfo (filePaths.first());
                    PrefManager::instance().setPref("Paths", "lp_open_media", fileInfo.absolutePath());
                }
            }
        } else {
            emit mediaDropped(filePaths);
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}


void PlayerWidget::restoreOverlayStackOrder()
{
    m_dragDropLogoWidget->raise();
    m_audioLogoWidget->raise();
    m_compositionWidget->raise();
    m_blackOpacityWidget->raise();
    m_drawingWidget->raise();   
}

void PlayerWidget::updateSingleOverlayGeom(QWidget* widget, bool isVisible){
    if (isVisible) {
        QRect globalRect(m_mediaWidget->mapToGlobal(QPoint(0, 0)), m_mediaWidget->size());
        widget->setGeometry(globalRect);
        
        restoreOverlayStackOrder();
    }  
}

void PlayerWidget::updateSingleLogoGeom(QWidget* widget, bool isVisible){
      if (isVisible) {
        QRect localRect(m_mediaWidget->mapTo(this, QPoint(0, 0)), m_mediaWidget->size());
        widget->setGeometry(localRect);
        
        restoreOverlayStackOrder();
    }    
}
