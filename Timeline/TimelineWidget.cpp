#include "Timeline/TimelineWidget.h"

#include "TimeFormatter.h"
#include "Project/ProjectManager.h"
#include "PrefManager.h"

#include "Timeline/Items/RulerItem.h"
#include "Timeline/Items/CursorItem.h"

#include "Timeline/ItemTypes.h"

#include "Shot.h"

#include "GenericDialog.h"
#include "ExtractSequenceWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include <algorithm>
#include "TimelineWidget.h"


/// @brief Créer une timeline avec les plan du projet
/// @param projectShots
/// @param parent
TimelineWidget::TimelineWidget(ThumbnailWorker* thumbnailWorker, Media* projectMedia, QVector<Shot>& projectShots, QWidget *parent, const int timelineWidth) : QWidget(parent)
{
    p_media = projectMedia;

    PrefManager& pref = PrefManager::instance();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);

    m_seekTimer = new QTimer(this);
    m_seekTimer->setSingleShot(true);

    connect(m_seekTimer, &QTimer::timeout, this, [this](){
        emit timelineSetPosition(m_vlcTime);
    });

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_sceneHeight);
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex); 

    m_mathManager = new TimelineMath(p_media->fps(), p_media->duration(), this);
    m_mathManager->fitToWidth(m_scene->width());

    m_abManager = new ABManager(m_scene, m_mathManager, this);
    connect(m_abManager, &ABManager::onPairCompleted, this, [this](){
        m_toPrevShotBtn->setEnabled(false);
        m_toNextShotBtn->setEnabled(false);
        emit disableTimeRelatedUI();
    });

    connect(m_abManager, &ABManager::onMarkersCleared, this,  [this](){
        m_toPrevShotBtn->setEnabled(true);
        m_toNextShotBtn->setEnabled(true);
        emit enableTimeRelatedUI();
    });


    connect(m_abManager, &ABManager::loopExtracted, this, [this](const QString& outputPath){
        exportDone(PrefManager::instance().getText("messagebox_extract_ab_loop_done"), outputPath);
    });
    connect(m_abManager, &ABManager::loopExtractionFailed, this, [this](){
        QMessageBox::warning(this, PrefManager::instance().getText("messagebox_error") , PrefManager::instance().getText("messagebox_extract_ab_loop_failed"));
    });

    QHBoxLayout* ButtonLayout = new QHBoxLayout();
    ButtonLayout->setContentsMargins(5, 0, 0, 0);

    m_abLoopBtn = new ToolbarButton(this, "abloop_white", pref.getText("tooltip_ab_loop"));
    connect(m_abLoopBtn, &ToolbarButton::pressed, this, &TimelineWidget::ABAction);
    ButtonLayout->addWidget(m_abLoopBtn);

    QFrame *btnSeparator = new QFrame();
    btnSeparator->setFrameShape(QFrame::VLine);
    btnSeparator->setFrameShadow(QFrame::Sunken);
    ButtonLayout->addWidget(btnSeparator);

    m_autoSegmentationBtn = new ToolbarButton(this, "auto_segmentation_white", pref.getText("tooltip_segmentation_auto"));
    connect(m_autoSegmentationBtn, &ToolbarButton::pressed, this, &TimelineWidget::autoSegmentation);
    ButtonLayout->addWidget(m_autoSegmentationBtn);

    m_splitShotBtn = new ToolbarButton(this, "split_shot_white", pref.getText("tooltip_split_shot"));
    connect(m_splitShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::splitShotAtCursor);
    ButtonLayout->addWidget(m_splitShotBtn);

    m_mergeWithPrevShotBtn = new ToolbarButton(this, "merge_left_white", pref.getText("tooltip_merge_with_prev_shot"));
    connect(m_mergeWithPrevShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::mergeWithPrevShotAction);
    ButtonLayout->addWidget(m_mergeWithPrevShotBtn);
    m_mergeWithPrevShotBtn->setEnabled(false);

    m_toPrevShotBtn = new ToolbarButton(this, "to_prev_shot_white", pref.getText("tooltip_to_prev_shot"));
    connect(m_toPrevShotBtn, &ToolbarButton::pressed, this, [this](){
        goToShot(m_shotManager->getCurrentShotId()-1);
    });
    ButtonLayout->addWidget(m_toPrevShotBtn);
    m_toPrevShotBtn->setEnabled(false);

    m_shotInfo = new ToolbarButton(this, "shot_detail_white", pref.getText("tooltip_shot_detail_button"));
    connect(m_shotInfo, &ToolbarButton::pressed, this, [](){
        emit SignalManager::instance().extensionToolbarDisplayShotDetail();
        emit SignalManager::instance().toggleNavPanel();
    });
    ButtonLayout->addWidget(m_shotInfo);

    m_toNextShotBtn = new ToolbarButton(this, "to_next_shot_white", pref.getText("tooltip_to_next_shot"));
    connect(m_toNextShotBtn, &ToolbarButton::pressed, this, [this](){
        goToShot(m_shotManager->getCurrentShotId()+1);
    });
    ButtonLayout->addWidget(m_toNextShotBtn);
    m_toNextShotBtn->setEnabled(false);

    m_mergeWithNextShotBtn = new ToolbarButton(this, "merge_right_white", pref.getText("tooltip_merge_with_next_shot"));
    connect(m_mergeWithNextShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::mergeWithNextShotAction);
    ButtonLayout->addWidget(m_mergeWithNextShotBtn);
    m_mergeWithNextShotBtn->setEnabled(false);

    m_exportBtn = new ToolbarButton(this, "export_white", pref.getText("tooltip_export"));
    connect(m_exportBtn, &ToolbarButton::pressed, &ProjectManager::instance(), &ProjectManager::exportProject);
    ButtonLayout->addWidget(m_exportBtn);

    ButtonLayout->addStretch(1);

    m_shotCountLabel = new QLabel(this);
    m_shotCountLabel->setContentsMargins(0, 0, 10, 0);
    QFont shotCountFont = m_shotCountLabel->font();
    shotCountFont.setItalic(true);
    m_shotCountLabel->setFont(shotCountFont);
    updateShotCount(static_cast<int>(projectShots.size()));

    ButtonLayout->addWidget(m_shotCountLabel);

    layout->addLayout(ButtonLayout);

    m_view = new TimelineView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // évite que le curseur ne soit completement effacé quand on scroll

    connect(m_view, &TimelineView::zoomRequested, this, &TimelineWidget::applyZoom);
    connect(m_view, &TimelineView::cursorPositionRequested, this, &TimelineWidget::moveCursor);
    connect(m_view, &TimelineView::itemRightClick, this, &TimelineWidget::itemRightClick);
    connect(m_view, &TimelineView::isDragging, this, [this](bool dragState){
        if(dragState) {
            m_wasPlayingBeforeDrag = m_isPlayerPlaying;
            emit playerPauseRequested();
        }
        else {
            if (m_wasPlayingBeforeDrag) emit playerPlayRequested();
        }

        m_isDraggingCursor = dragState;
    });
    connect(m_view, &TimelineView::abMarkerDragged, m_abManager, &ABManager::dragMarker);

    layout->addWidget(m_view);

    m_shotManager = new ShotManager(m_scene, m_view, m_mathManager, thumbnailWorker, projectMedia, projectShots, this);
    computeMediaAmplitudes(projectMedia->filePath());

    connect(m_view, &TimelineView::shotSelectionRequested, m_shotManager, &ShotManager::toggleSelection);
    connect(m_shotManager, &ShotManager::updateShotDetailRequested, this, &TimelineWidget::updateShotDetailRequest );
    connect(m_shotManager, &ShotManager::showMergeWithPreviousShotAction, this, &TimelineWidget::updateShowMergeWithPreviousShot );
    connect(m_shotManager, &ShotManager::showMergeWithNextShotAction, this, &TimelineWidget::updateShowMergeWithNextShot  );
    connect(m_shotManager, &ShotManager::shotCountUpdated, this, &TimelineWidget::updateShotCount);
    connect(m_shotManager, &ShotManager::shotsExtractionFinished, this, [this](const QString& outputPath){
        exportDone(PrefManager::instance().getText("messagebox_extract_selected_shots_completed"), outputPath);
    });
    connect(m_shotManager, &ShotManager::shotsExtractionFailed, this, [this](){
        QMessageBox::warning(this, PrefManager::instance().getText("messagebox_error") , PrefManager::instance().getText("messagebox_extract_shots_failed"));
    });

    m_annotItemManager = new AnnotationItemManager(m_scene, m_view, m_mathManager, this);
    connect(m_view, &TimelineView::annotationHandleDragged, m_annotItemManager, &AnnotationItemManager::onAnnotationHandleDragged);

    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight, m_minPxBetweenTicks, m_mathManager->pixelsPerMs(), projectMedia->duration(), projectMedia->fps());
    m_ruler->setPos(0, 0);
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_sceneHeight);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

    if(timelineWidth != 0){

        double minPixelsPerMs =
        double(timelineWidth) / projectMedia->duration();

        m_mathManager->setPixelsPerMs(minPixelsPerMs);

        updateTimelineGeometry();

    }

    // ProjectManager::instance().setSaveNeeded();
}

TimelineWidget::~TimelineWidget()
{
    if(m_segmThread && m_segmThread->isRunning()){
        m_segmThread->requestInterruption();
        m_segmThread->wait();
    }

    if (m_audioComputeProcess) {
        m_audioComputeProcess->kill();
        m_audioComputeProcess->waitForFinished();

        m_audioComputeProcess->deleteLater();
        m_audioComputeProcess = nullptr;
    }
}

QVector<Shot> TimelineWidget::getTimelineData()
{
    return m_shotManager->shotItemsData();
}

void TimelineWidget::setTimelineData(QVector<Shot> shots)
{
    m_shotManager->setShotItemsData(shots);
}

void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    fitSceneToViewport();
}

bool TimelineWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
        QTimer::singleShot(0, this, SLOT(fitSceneToViewport())); 
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void TimelineWidget::updateShotCount(int shotCount)
{
    m_shotCountLabel->setText(QString::number(shotCount) + " " + PrefManager::instance().getText("timeline_shot_count_label"));
}

/// @brief Agrandi ou rétrécit la scène ou fonction de la molette, recalcul ensuite la position de graphics items
/// @param zoomFactor
/// @param mouseX
void TimelineWidget::applyZoom(double zoomFactor, int mouseX) {
    double currentTimeUnderMouse = (m_view->horizontalScrollBar()->value() + mouseX) / m_mathManager->pixelsPerMs();
    double newPixelsPerMs = m_mathManager->pixelsPerMs() * zoomFactor;

    double minWidth = m_view->viewport()->width(); // on va limiter le dézom pour qu'au minimum la scene fait la talle du viewport
    double maxWidth = std::numeric_limits<int>::max() - 1000000.0; // en cas de vidéo très très longue le zoom peut causer des problèmes, -1 000 000 pour de la marge au cas ou

    double fps = m_mathManager->fps();
    auto duration  = m_mathManager->duration();
    if (fps > 0) {
        double frameMs = 1000.0 / fps;
        double totalFrames = static_cast<double>(duration) / frameMs;

        maxWidth = std::min( maxWidth, (totalFrames * m_minPxBetweenTicks));
        // la scene fera au maximum : nb de frames * l'espacement entre les ticks

        // si vidéo courte, le taille du view port peut être supérieur à maxWidth
        if (maxWidth < minWidth) maxWidth = minWidth;
    }

    // limite toujours la taille mais via le ratio et plus la taille de la scene
    double minRatio = minWidth / static_cast<double>(duration);
    double maxRatio = maxWidth / static_cast<double>(duration);

    m_mathManager->setPixelsPerMs(std::clamp(newPixelsPerMs, minRatio, maxRatio));

    updateTimelineGeometry();

    double newPixelPos = currentTimeUnderMouse * m_mathManager->pixelsPerMs();
    m_view->horizontalScrollBar()->setValue(qRound(newPixelPos - mouseX));
}

void TimelineWidget::fitSceneToViewport(){
    int viewportHeight = m_view->viewport()->height();
    int viewportWidth = m_view->viewport()->width();

    if (m_scene) {
        if(m_sceneWidth < viewportWidth){
            m_scene->setSceneRect(0, 0, viewportWidth, viewportHeight);
            m_mathManager->fitToWidth(m_scene->width());
        }else {
            m_scene->setSceneRect(0, 0, m_sceneWidth, viewportHeight);
        }

        updateTimelineGeometry();
    }
}

void TimelineWidget::updateTimelineGeometry()
{
    m_sceneWidth = m_mathManager->duration() * m_mathManager->pixelsPerMs();

    m_scene->setSceneRect(0, 0, m_sceneWidth, m_scene->height());

    m_ruler->setSize(
        m_sceneWidth,
        m_rulerHeight,
        m_mathManager->pixelsPerMs()
        );

    m_abManager->updateMarkersPosition();

    if (m_audioVisualizer)
        m_audioVisualizer->setWidth(m_sceneWidth);

    updateCursorPos(m_vlcTime);

    m_shotManager->updateShotItemsPosition();
    m_annotItemManager->updateAnnotItemsPosition();
}


/// @brief déplace le curseur si l'ab loop est active, clamp au min et max, met à jour le temps et et envoie à la toolbar le nouveau temps
/// @param cursorPosX
void TimelineWidget::moveCursor(double newCursorPosX)
{
    int64_t newCursorTime = m_mathManager->posToTimeSnapped(newCursorPosX);
    auto clampedTime = m_abManager->clampToLoopRange(newCursorTime);

    int64_t targetTime = clampedTime.value_or(newCursorTime);

    if (targetTime == m_vlcTime) {
        qDebug() << "skip movecursor";
        return; 
    }

    bool timeClamped = clampedTime.has_value() && (clampedTime.value() != newCursorTime);

    m_vlcTime = targetTime;

    m_cursor->setPos(m_mathManager->timeToPos(m_vlcTime), m_cursor->pos().y());

    emit timelineSliderPositionRequested(m_vlcTime);

    if(!m_seekTimer->isActive() && !timeClamped){
        m_seekTimer->start(m_seekPendingTime);
    }

    m_shotManager->updateCurrentShot(m_vlcTime);
}

void TimelineWidget::itemRightClick(QPoint globalPos, QGraphicsItem * item)
{
    switch( item->type() ) {
        case SLV::TypeAudioShotItem :
        case SLV::TypeShotItem :
            ShotItem* shotItem = static_cast<ShotItem*>(item);
            showContextMenuForShot(globalPos, shotItem);
            break;
    }
}



void TimelineWidget::showContextMenuForShot(const QPoint& globalPos, ShotItem* item )
{
    QMenu menu;
    QAction *actionSplit = menu.addAction(PrefManager::instance().getText("timeline_split_shot_at_cursor"));
    QAction *actionExtractShot = menu.addAction(PrefManager::instance().getText("timeline_extract_selected_shot"));
    QAction *mergeWithPreviousShot = nullptr;
    QAction *mergeWithNextShot = nullptr;
    QAction *actionAB = nullptr;
    QAction *deleteABMarkers = nullptr;
    QAction *actionExtractAB = nullptr;
    QAction *actionExtractShotsSelected = nullptr;
    // Pour ouvrir le nav panel sur le plan sélectionné
    QAction* actionOpenShotInfo = menu.addAction(PrefManager::instance().getText("tooltip_shot_detail_button"));

    if(m_showMergeWithPrevShotBtn){
        mergeWithPreviousShot = menu.addAction(PrefManager::instance().getText("timeline_merge_with_previous_shot"));
    }
    if(m_showMergeWithNextShotBtn){
        mergeWithNextShot = menu.addAction(PrefManager::instance().getText("timeline_merge_with_next_shot"));
    }

    switch (m_abManager->getMarkerCount())
    {
    case 0:
        actionAB = menu.addAction(PrefManager::instance().getText("timeline_ab_action_0"));
        break;
    case 1:
        actionAB = menu.addAction(PrefManager::instance().getText("timeline_ab_action_1"));
        deleteABMarkers = menu.addAction(PrefManager::instance().getText("timeline_ab_action_2"));
        break;
    case 2:
        actionAB = menu.addAction(PrefManager::instance().getText("timeline_ab_action_2"));
        actionExtractAB = menu.addAction(PrefManager::instance().getText("timeline_ab_extract"));
        break;
    }

    if ( m_shotManager->getNbShotsSelected() > 0 ) {
        actionExtractShotsSelected = menu.addAction(PrefManager::instance().getText("timeline_extract_selected_shots"));
    }

    QAction *selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;
    
    if (selectedAction == actionSplit){
        splitShotAtCursor();
    } else if (selectedAction == actionAB){
        ABAction();
    } else if (selectedAction == deleteABMarkers){
        m_abManager->deleteMarkers();
    } else if (selectedAction == actionExtractAB){
        m_abManager->extractLoop();
    } else if (selectedAction == mergeWithPreviousShot){
        mergeWithPrevShotAction();
    } else if(selectedAction == mergeWithNextShot){
        mergeWithNextShotAction();
    } else if(selectedAction == actionExtractShot){
        QString saveSequencePath = QFileDialog::getSaveFileName(this, tr("Extract sequence"),
            PrefManager::instance().getPref("Paths", "lp_extract_sequence")
                + '/' + p_media->fileName()+"_"+TimeFormatter::fileFormatMsToHHMMSSFF(item->shot().start, p_media->fps())+"_"+TimeFormatter::fileFormatMsToHHMMSSFF(item->shot().end, p_media->fps()));
        if(saveSequencePath != ""){
            QProcess* sequenceExtractor = SequenceExtractionHelper::extractSequence(p_media->filePath(), item->shot().start, item->shot().end, saveSequencePath.split('.')[0] + '.' + p_media->fileExtension());
            connect(sequenceExtractor, &QProcess::finished, this, [this, sequenceExtractor, saveSequencePath](){
                if (sequenceExtractor->exitStatus() == QProcess::NormalExit && sequenceExtractor->exitCode() == 0){
                    exportDone(PrefManager::instance().getText("messagebox_extract_shot_completed"), saveSequencePath);
                } else {
                    QMessageBox::warning(this, PrefManager::instance().getText("messagebox_error") , PrefManager::instance().getText("messagebox_extract_shot_failed"));
                }
            });
        }
    } else if(selectedAction == actionOpenShotInfo){
        moveCursor(m_mathManager->timeToPos(item->shot().start));
        emit SignalManager::instance().extensionToolbarDisplayShotDetail();
        emit SignalManager::instance().toggleNavPanel();
    }else if (selectedAction == actionExtractShotsSelected){

        QFileInfo fileInfo (p_media->filePath());

        auto& prefManager = PrefManager::instance();
        QString saveRecordPath = QFileDialog::getSaveFileName(
            this,
            prefManager.getText("dialog_capture") + fileInfo.fileName() + "_record",
            prefManager.getPref("Paths", "lp_capture")
        );

        if (saveRecordPath.isEmpty()){
            qDebug() << "[TIMELINEWIDGET] Enregistrement des shots sélectionnés annulé";
            return;
        } 

        m_shotManager->extractShotsSelected(saveRecordPath +"."+ fileInfo.suffix());
    }
}

// public slots

/// @brief Reçoit le temps vlc et met à jour la position en conséquence
/// @param vlcTime
void TimelineWidget::updateCursorPos(int64_t vlcTime){

    if(m_isDraggingCursor || !m_abManager) return;

    auto restartTime = m_abManager->getLoopRestartTime(vlcTime);

    if(restartTime.has_value()){ // si on a une value, on a dépassé le marqueur B, on revient a A
        m_vlcTime = restartTime.value();
        m_shotManager->updateCurrentShot(m_vlcTime);
        emit timelineSetPosition(m_vlcTime);
    }else {
        // Pendant le chargement du restart, on garde le curseur sur A au lieu de
        // l'afficher sur l'undershoot (temps sous A rapporté par libvlc à vitesse lente).
        m_vlcTime = m_abManager->getDisplayHoldTime(vlcTime).value_or(vlcTime);
    }

    m_cursor->setX(m_mathManager->timeToPos(m_vlcTime));
    m_shotManager->updateCurrentShot(m_vlcTime);
}


void TimelineWidget::updateCursorVisually(int sliderValue) {
    m_cursor->setX(m_mathManager->timeToPos(sliderValue));
    m_shotManager->updateCurrentShot(sliderValue);
}

void TimelineWidget::goToShot(int idShot){
    if(m_abManager->getMarkerCount() == 2) return;

    auto timeStart = m_shotManager->getStartTimeOf(idShot);
    if(timeStart.has_value()){
        emit timelineSetPosition(timeStart.value());
    }
}

void TimelineWidget::initShotDetail(){
    m_shotManager->initShotDetail();
}


// private slots

void TimelineWidget::ABAction() {
    int64_t markerTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_abManager->cycleMarkers(markerTime, m_sceneHeight);
}

void TimelineWidget::splitShotAtCursor()
{
    int64_t cutTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_shotManager->splitShotAt(cutTime);
    emit saveNeeded();
}

void TimelineWidget::mergeWithPrevShotAction()
{
    int64_t cursorTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_shotManager->mergeCurrentWithPrevShot(cursorTime);
    emit saveNeeded();
}

void TimelineWidget::mergeWithNextShotAction()
{
    int64_t cursorTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_shotManager->mergeCurrentWithNextShot(cursorTime);
    emit saveNeeded();
}


void TimelineWidget::updateShowMergeWithPreviousShot(bool state)
{
    m_mergeWithPrevShotBtn->setEnabled(state);
    m_toPrevShotBtn->setEnabled(state);
    m_showMergeWithPrevShotBtn = state;
}

void TimelineWidget::updateShowMergeWithNextShot(bool state)
{
    m_mergeWithNextShotBtn->setEnabled(state);
    m_toNextShotBtn->setEnabled(state);
    m_showMergeWithNextShotBtn = state;
}

void TimelineWidget::autoSegmentation(){

    auto& txtManager = PrefManager::instance();
    const QString& mediaPath = ProjectManager::instance().mediaPath();
    if(mediaPath.isEmpty()){
        qDebug() << "Project media path est vide";
        return;
    }

    SLV::showGenericDialog(
        this,
        txtManager.getText("dialog_auto_segmentation_title"),
        txtManager.getText("dialog_auto_segmentation_text"),

        [this, mediaPath]() {
            auto& txtManager = PrefManager::instance();
            m_segmThread = new SegmentationThread(mediaPath, this);

            QProgressDialog* progressDialog = new QProgressDialog(txtManager.getText("timeline_dialog_text_auto_segmentation"), txtManager.getText("generic_dialog_btn_cancel"), 0, 100, nullptr);
            progressDialog->setWindowTitle(txtManager.getText("timeline_dialog_title_auto_segmentation"));
            progressDialog->setWindowModality(Qt::WindowModal);

            connect(progressDialog, &QProgressDialog::canceled, m_segmThread, &QThread::requestInterruption);

            connect(m_segmThread, &SegmentationThread::progress, progressDialog, &QProgressDialog::setValue);

            connect(m_segmThread, &SegmentationThread::segmentationFinished, this, [this, progressDialog] (std::vector<int> cuts) {

                progressDialog->close();
                progressDialog->deleteLater();

                if( ! cuts.empty()){
                    this->m_shotManager->createShotItemsFromCuts(cuts);
                    emit this->saveNeeded();

                    QString text = PrefManager::instance().getText("timeline_dialog_text_auto_segmentation_finished") + QString::number(cuts.size() + 1);
                    QMessageBox msg(this);
                    msg.setWindowTitle(PrefManager::instance().getText("timeline_dialog_title_auto_segmentation"));
                    msg.setStandardButtons(QMessageBox::Ok);
                    msg.setInformativeText(text);
                    msg.setIcon(QMessageBox::Information);
                    msg.exec();
                }
            });

            connect(m_segmThread, &QThread::finished, m_segmThread, &QObject::deleteLater);

            progressDialog->show();
            m_segmThread->start();
            m_segmThread->setPriority(QThread::HighPriority);
        },
        nullptr,
        nullptr
    );

}

void TimelineWidget::computeMediaAmplitudes(const QString &mediaPath)
{
    m_audioComputeProcess = new QProcess(this);

    QStringList args = {
        "-loglevel", "quiet",
        "-i", mediaPath,
        "-f", "s16le",
        "-ac", "1",
        "-ar", "44100",
        "pipe:1"
    };

    m_audioBuffer.clear();
    m_amplitudeList.clear();

    connect(m_audioComputeProcess, &QProcess::readyReadStandardOutput, this, [this]() {

        m_audioBuffer.append(m_audioComputeProcess->readAllStandardOutput());

        const int frameSamples = 1024;
        const int frameBytes = frameSamples * sizeof(int16_t);

        while (m_audioBuffer.size() >= frameBytes) {

            const int16_t *samples =
                reinterpret_cast<const int16_t*>(m_audioBuffer.constData());

            double sum = 0.0;

            for (int i = 0; i < frameSamples; ++i) {
                sum += std::abs((double)samples[i]);
            }

            double amplitude = (sum / frameSamples) / 32768.0;

            m_amplitudeList.append(amplitude);

            m_audioBuffer.remove(0, frameBytes);
        }
    });

    connect(m_audioComputeProcess, &QProcess::readyReadStandardError, this, [this]() {
        m_audioComputeProcess->readAllStandardError();
    });

    connect(m_audioComputeProcess, &QProcess::finished, this, [this](){
        initAudioVisualizer();
    });

    QString appDir = QCoreApplication::applicationDirPath();
    QString ffmpegExe;
#if defined(Q_OS_WIN)
    ffmpegExe = appDir + "/bin/ffmpeg.exe";
#elif defined(Q_OS_MAC)
    ffmpegExe = appDir + "/../Resources/ffmpeg/ffmpeg";
#else
    ffmpegExe = appDir + "/bin/ffmpeg";
#endif

    m_audioComputeProcess->start(ffmpegExe, args);
    //m_audioComputeProcess->start(QString(FFMPEG_EXECUTABLE), args);

}

void TimelineWidget::initAudioVisualizer()
{
    m_audioVisualizer = new AudioVisualizerItem(m_amplitudeList, m_sceneWidth);
    m_audioVisualizer->setPos(0, 0);
    m_scene->addItem(m_audioVisualizer);
}



void TimelineWidget::exportDone(const QString& text, const QString &outputPath)
{
    QMessageBox msg;
    QPushButton *openDirBtn = msg.addButton(
        PrefManager::instance().getText("open_file_directory"),
        QMessageBox::AcceptRole
    );
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setInformativeText(text);
    msg.setIcon(QMessageBox::Information);
    msg.exec(); 

    if (msg.clickedButton() == openDirBtn) {
        QFileInfo fi(outputPath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.dir().path()));
    }
}
