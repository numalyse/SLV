#include "Timeline/TimelineWidget.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"
#include "TextManager.h"

#include "Timeline/Items/RulerItem.h"
#include "Timeline/Items/CursorItem.h"

#include "Timeline/ItemTypes.h"

#include "Shot.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <algorithm> 
#include "TimelineWidget.h"


/// @brief Créer une timeline avec les plan du projet
/// @param projectShots 
/// @param parent 
TimelineWidget::TimelineWidget(QVector<Shot>& projectShots, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 

    auto fps = ProjectManager::instance().projet()->media->fps();
    auto duration = ProjectManager::instance().projet()->media->duration();

    if(fps == 0.0 || duration == 0){
        qDebug() << "Creation timeline : Fps ou durée du film = 0";
        return;
    }

    m_seekTimer = new QTimer(this);
    m_seekTimer->setSingleShot(true);

    connect(m_seekTimer, &QTimer::timeout, this, [this](){
        emit timelineSetPosition(m_vlcTime);
    });

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_sceneHeight);

    m_mathManager = new TimelineMath(fps, duration, this);
    m_mathManager->fitToWidth(m_scene->width());

    m_abManager = new ABManager(m_scene, m_mathManager, this);
    connect(m_abManager, &ABManager::ABLoopOn, this, &TimelineWidget::disableTimeRelatedUI);
    connect(m_abManager, &ABManager::ABLoopOff, this, &TimelineWidget::enableTimeRelatedUI);

    QHBoxLayout* ButtonLayout = new QHBoxLayout();
    ButtonLayout->setContentsMargins(0, 0, 0, 0); 

    m_splitShotBtn = new ToolbarButton(this, "split_shot_white", TextManager::instance().get("tooltip_split_shot"));
    connect(m_splitShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::splitShotAtCursor);
    ButtonLayout->addWidget(m_splitShotBtn);

    m_abLoopBtn = new ToolbarButton(this, "abloop_white", TextManager::instance().get("tooltip_ab_loop"));
    connect(m_abLoopBtn, &ToolbarButton::pressed, this, &TimelineWidget::ABAction);
    ButtonLayout->addWidget(m_abLoopBtn);

    m_mergeWithPrevShotBtn = new ToolbarButton(this, "abloop_white", TextManager::instance().get("tooltip_merge_with_prev_shot"));
    connect(m_mergeWithPrevShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::mergeWithPrevShotAction);
    ButtonLayout->addWidget(m_mergeWithPrevShotBtn);
    m_mergeWithPrevShotBtn->hide();

    m_mergeWithNextShotBtn = new ToolbarButton(this, "abloop_white", TextManager::instance().get("tooltip_merge_with_next_shot"));
    connect(m_mergeWithNextShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::mergeWithNextShotAction);
    ButtonLayout->addWidget(m_mergeWithNextShotBtn);
    m_mergeWithNextShotBtn->hide();

    ButtonLayout->addStretch(1);
    layout->addLayout(ButtonLayout);

    m_view = new TimelineView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop); 
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // évite que le curseur ne soit pas completement effacé quand on scroll
    
    connect(m_view, &TimelineView::zoomRequested, this, &TimelineWidget::applyZoom);
    connect(m_view, &TimelineView::cursorPositionRequested, this, &TimelineWidget::moveCursor);
    connect(m_view, &TimelineView::itemLeftClick, this, &TimelineWidget::itemLeftClick);
    connect(m_view, &TimelineView::itemRightClick, this, &TimelineWidget::itemRightClick);
    connect(m_view, &TimelineView::isDragging, this, [this](bool dragState){
        m_isDraggingCursor = dragState;
    });

    layout->addWidget(m_view);

    m_shotManager = new ShotManager(m_scene, m_view, m_mathManager, projectShots, this);

    connect(m_shotManager, &ShotManager::updateShotDetailRequested, this, &TimelineWidget::updateShotDetailRequest );
    connect(m_shotManager, &ShotManager::showMergeWithPreviousShotAction, this, &TimelineWidget::updateShowMergeWithPreviousShot );
    connect(m_shotManager, &ShotManager::showMergeWithNextShotAction, this, &TimelineWidget::updateShowMergeWithNextShot  );

    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight, m_minPxBetweenTicks, m_mathManager->pixelsPerMs(), duration, fps);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_sceneHeight);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

}


void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); 

    int viewportHeight = m_view->viewport()->height();
    int viewportWidth = m_view->viewport()->width();

    if (m_scene) {
        if(m_sceneWidth < viewportWidth){
            m_scene->setSceneRect(0, 0, viewportWidth, viewportHeight);
            m_mathManager->fitToWidth(m_scene->width());
        }else {
            m_scene->setSceneRect(0, 0, m_sceneWidth, viewportHeight);
        }

        if(m_ruler){
            m_ruler->setSize(m_sceneWidth, m_rulerHeight, m_mathManager->pixelsPerMs());
        }
        
        m_abManager->updateMarkersPosition();

        m_shotManager->updateShotItemsPosition();
        
        if(m_cursor){
            updateCursorPos(m_vlcTime);
        }
    }
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

    m_sceneWidth = duration * m_mathManager->pixelsPerMs();
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_scene->height());

    m_ruler->setSize(m_sceneWidth, m_rulerHeight, m_mathManager->pixelsPerMs());

    m_abManager->updateMarkersPosition();

    updateCursorPos(m_vlcTime);

    m_shotManager->updateShotItemsPosition();

    double newPixelPos = currentTimeUnderMouse * m_mathManager->pixelsPerMs();
    m_view->horizontalScrollBar()->setValue(qRound(newPixelPos - mouseX));
}

/// @brief retourne le temps en ms de la frame le plus proche du curseur
/// @return 


/// @brief déplace le curseur si l'ab loop n'est pas activé, met à jour le temps et et envoie à la toolbar le nouveau temps
/// @param cursorPosX 
void TimelineWidget::moveCursor(double newCursorPosX){
    int64_t newCursorTime = m_mathManager->posToTime(newCursorPosX);
    auto restartTime = m_abManager->getLoopRestartTime(newCursorTime);

    if(restartTime.has_value()){ 
        m_vlcTime = restartTime.value();
        m_cursor->setPos(m_mathManager->timeToPos(m_vlcTime), m_cursor->pos().y());
        return;
    }

    m_vlcTime = m_mathManager->posToTimeSnapped(newCursorPosX);
    m_cursor->setPos(m_mathManager->timeToPos(m_vlcTime), m_cursor->pos().y());

    emit timelineSliderPositionRequested(m_vlcTime); // visual update for slider

    if( ! m_seekTimer->isActive() ){ // limite les appeles à setTime VLC
        m_seekTimer->start(m_seekPendingTime);
    }

    m_shotManager->updateCurrentShot(m_vlcTime);
}

/// @brief retrouve le type d'object sur lequel on a cliqué, si c'est un plan, déplace le curseur au debut du plan
/// @param item 
void TimelineWidget::itemLeftClick(QGraphicsItem * item)
{
    switch( item->type() ) {
        case SLV::TypeShotItem: 
            ShotItem* shotItem = static_cast<ShotItem*>(item);
            moveCursor(m_mathManager->timeToPos(shotItem->shot().start));
            break;
    }
}

void TimelineWidget::itemRightClick(QPoint globalPos, QGraphicsItem * item)
{
    switch( item->type() ) {
        case SLV::TypeShotItem: 
            ShotItem* shotItem = static_cast<ShotItem*>(item);
            showContextMenuForShot(globalPos, shotItem);
            break;
    }
}



void TimelineWidget::showContextMenuForShot(const QPoint& globalPos, ShotItem* item )
{
    QMenu menu;
    QAction *actionSplit = menu.addAction(TextManager::instance().get("timeline_split_shot_at_cursor"));
    QAction *actionAB = nullptr;
    QAction *actionExtractAB = nullptr;
    // ajouter une action pour supprimer le marqueur seul

    switch (m_abManager->getMarkerCount())
    {
    case 0:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_0"));
        break;
    case 1:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_1"));
        // ajouter une action pour supprimer le marqueur seul
        break;
    case 2:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_2"));
        actionExtractAB = menu.addAction(TextManager::instance().get("timeline_ab_extract"));
        break;
    }

    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == actionSplit) {
        splitShotAtCursor();
    }else if (selectedAction == actionAB){
        ABAction();
    }else if (selectedAction == actionExtractAB){
        qDebug() << "Extract ab segment";
    }
}

// public slots 

/// @brief Reçoit le temps vlc et met à jour la position en conséquence
/// @param vlcTime 
void TimelineWidget::updateCursorPos(int64_t vlcTime){

    if(m_isDraggingCursor) return;

    auto restartTime = m_abManager->getLoopRestartTime(vlcTime);

    if(restartTime.has_value()){ // si on a une value, on a dépassé le marqueur B, on revient a A
        m_vlcTime = restartTime.value();
        m_shotManager->updateCurrentShot(m_vlcTime);
        emit timelineSetPosition(m_vlcTime);
    }else {
        m_vlcTime = vlcTime;
    }

    m_cursor->setX(m_mathManager->timeToPos(m_vlcTime));
    m_shotManager->updateCurrentShot(m_vlcTime);
}


void TimelineWidget::updateCursorVisually(int sliderValue) {
    m_cursor->setX(m_mathManager->timeToPos(sliderValue));
    m_shotManager->updateCurrentShot(sliderValue);
}

void TimelineWidget::goToShot(int idShot){
    auto timeStart = m_shotManager->getStartTimeOf(idShot);
    if(timeStart.has_value()){
        emit timelineSetPosition(timeStart.value());
    }
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
}

void TimelineWidget::mergeWithPrevShotAction()
{
    int64_t cursorTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_shotManager->mergeCurrentWithPrevShot(cursorTime);
}

void TimelineWidget::mergeWithNextShotAction()
{
    int64_t cursorTime = m_mathManager->posToTimeSnapped(m_cursor->pos().x());
    m_shotManager->mergeCurrentWithNextShot(cursorTime);
}


void TimelineWidget::updateShowMergeWithPreviousShot(bool state)
{
    if (state){
        m_mergeWithPrevShotBtn->show();
    }else {
        m_mergeWithPrevShotBtn->hide();
    } 
    m_showMergeWithPrevShotBtn = state;
}

void TimelineWidget::updateShowMergeWithNextShot(bool state)
{
    if (state){
        m_mergeWithNextShotBtn->show();
    }else {
        m_mergeWithNextShotBtn->hide();
    } 
    m_showMergeWithNextShotBtn = state;
}