#include "TimelineWidget.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"

#include "RulerItem.h"
#include "CursorItem.h"

#include "Shot.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>

#include <algorithm> 

TimelineWidget::TimelineWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 

    m_fps = ProjectManager::instance().projet()->media->fps();
    m_duration = ProjectManager::instance().projet()->media->duration();
    if(m_fps == 0.0 || m_duration == 0){
        qDebug() << "Creation timeline : Fps ou durée du film = 0";
        return;
    }

    m_pixelsPerMs = m_sceneWidth / static_cast<double>(m_duration);

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_sceneHeight);

    m_view = new TimelineView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop); 
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // évite que le curseur ne soit pas completement effacé quand on scroll
    connect(m_view, &TimelineView::zoomRequested, this, &TimelineWidget::applyZoom);
    connect(m_view, &TimelineView::cursorPositionRequested, this, &TimelineWidget::moveCursor);

    layout->addWidget(m_view); 

    m_splitShotBtn = new ToolbarButton(this);
    connect(m_splitShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::splitCurrentShotItem);
    layout->addWidget(m_splitShotBtn);

    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight, m_minPxBetweenTicks, m_pixelsPerMs, m_duration, m_fps);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_sceneHeight);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

    int startShotHeight = m_sceneHeight - m_rulerHeight;
    for ( auto& IShot : ProjectManager::instance().projet()->shots ){
        ShotItem* shot = new ShotItem(IShot, startShotHeight); // stocke ptr non owner dans les shotItem => modification dans projet modifie le shot et inversement
        m_scene->addItem(shot);
        m_shotItems.append(shot);
    }

    connect(&SignalManager::instance(), &SignalManager::simpleToolbarUpdateCursorPosition, this, &TimelineWidget::updateCursorPos);
    connect(this, &TimelineWidget::timelineSetPosition, &SignalManager::instance(), &SignalManager::timelineSetPosition);
}

void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); 

    int viewportHeight = m_view->viewport()->height();

    if (m_scene) {
        m_scene->setSceneRect(0, 0, m_sceneWidth, viewportHeight);
    }
    if (m_cursor) {
        //m_cursor->setHeight(viewportHeight);
    }
}


void TimelineWidget::updateCursorPos(int64_t vlcTime){
    m_vlcTime = vlcTime;

    double posCursor = static_cast<double>(vlcTime) * m_pixelsPerMs; 
    
    m_cursor->setPos(posCursor, 0);

    updateCurrentShot();
}

void TimelineWidget::updateCurrentShot(){
    int shotItemCount = static_cast<int>(m_shotItems.size());

    Q_ASSERT(shotItemCount >= 1);
    if (shotItemCount <= 0 ) return;

    ShotItem* closestLeftShot = m_shotItems[0];
    int64_t distanceToClosest = m_vlcTime - closestLeftShot->shot()->start;

    for (int IShotItem = 1; IShotItem < shotItemCount; ++IShotItem){
        
        int64_t currentShotStart = m_shotItems[IShotItem]->shot()->start;

        if( m_vlcTime < currentShotStart ) break; // si le vlc time < start on est avant le plan donc on quitte

        int64_t distance = m_vlcTime - currentShotStart;
        if(distance < distanceToClosest){
            distanceToClosest = distance;
            closestLeftShot = m_shotItems[IShotItem];
        }
    }
    
    if(m_currentShotItem != closestLeftShot){
        m_currentShotItem = closestLeftShot;
        emit updateShotDetailRequested(m_currentShotItem->shot());
    }
}


void TimelineWidget::applyZoom(double zoomFactor) {
    double newPixelsPerMs = m_pixelsPerMs * zoomFactor;

    double minWidth = m_view->viewport()->width(); // on va limiter le dézom pour qu'au minimum la scene fait la talle du viewport 
    double maxWidth = std::numeric_limits<int>::max() - 1000000.0; // en cas de vidéo très très longue le zoom peut causer des problèmes, -1 000 000 pour de la marge au cas ou
    
    double fps = ProjectManager::instance().projet()->media->fps();
    if (fps > 0) {
        double frameMs = 1000.0 / fps;
        double totalFrames = static_cast<double>(m_duration) / frameMs;
        
        maxWidth = std::min( maxWidth, (totalFrames * m_minPxBetweenTicks)); 
        // la scene fera au maximum : nb de frames * l'espacement entre les ticks
        
        // si vidéo courte, le taille du view port peut être supérieur à maxWidth
        if (maxWidth < minWidth) maxWidth = minWidth;
    }

    // limite toujours la taille mais via le ratio et plus la taille de la scene
    double minRatio = minWidth / static_cast<double>(m_duration);
    double maxRatio = maxWidth / static_cast<double>(m_duration);

    m_pixelsPerMs = std::clamp(newPixelsPerMs, minRatio, maxRatio);

    m_sceneWidth = m_duration * m_pixelsPerMs;
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_scene->height());

    m_ruler->setSize(m_sceneWidth, m_rulerHeight, m_pixelsPerMs);
    updateCursorPos(m_vlcTime);
}


int64_t TimelineWidget::timeAtCursor() {
    double cursorPos = m_cursor->pos().x();
    
    double rawMs = cursorPos / m_pixelsPerMs; 

    // snap à la frame la plus proche
    if (m_fps > 0) {
        double frameMs = 1000.0 / m_fps;
        double nearestFrameIndex = std::round(rawMs / frameMs);
        rawMs = nearestFrameIndex * frameMs; 
    }

    int64_t finalMs = static_cast<int64_t>(rawMs);
    return std::clamp(finalMs, static_cast<int64_t>(0), m_duration);
}

void TimelineWidget::moveCursor(double cursorPosX){
    m_cursor->setPos(cursorPosX,0);
    emit timelineSetPosition(timeAtCursor());
}

void TimelineWidget::splitCurrentShotItem(){
    Shot* currShot = m_currentShotItem->shot();

    qDebug() << "Ms au curseur " << timeAtCursor();
    qDebug() << "Temps au curseur " << TimeFormatter::msToHHMMSSFF( timeAtCursor(), ProjectManager::instance().projet()->media->fps());

    //int64_t currShotEnd = timeAtCursor-1; // la fin du plan est la position du curseur-1



}