#include "TimelineWidget.h"

#include "ProjectManager.h"
#include "RulerItem.h"
#include "CursorItem.h"

#include "Shot.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>

TimelineWidget::TimelineWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_sceneHeight);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop); 
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->viewport()->installEventFilter(this);
    m_view->installEventFilter(this);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // évite que le curseur ne soit pas completement effacé quand on scroll
    layout->addWidget(m_view); 

    m_testButton = new ToolbarButton(this);
    layout->addWidget(m_testButton);

    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight, m_minPxBetweenTicks);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_sceneHeight);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

    int startShotHeight = m_sceneHeight - m_rulerHeight;
    for ( auto& IShot : ProjectManager::instance().projet()->shots ){
        ShotItem* shot = new ShotItem(IShot, startShotHeight);
        m_scene->addItem(shot);
        m_shotItems.append(shot);
    }
    

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
    int64_t duration = ProjectManager::instance().projet()->media->duration();

    if (duration <= 0) return;

    double ratio = static_cast<double>(vlcTime) / static_cast<double>(duration);
    
    int posCursor = static_cast<int>(ratio * m_scene->width()); 
    
    m_cursor->setPos(posCursor, 0);

}


bool TimelineWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_view->viewport()) {
        if (event->type() == QEvent::Wheel ) {
            // infos de la molette
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            QPoint numDegrees = wheelEvent->angleDelta() / 8;

            double viewportX = wheelEvent->position().x();
            double scrollbarValue = (m_view->horizontalScrollBar()->isVisible()) ? m_view->horizontalScrollBar()->value() : 0 ;
            qDebug() << "Timeline Scrollbar event position x : " << viewportX ;
            qDebug() << "Timeline Scrollbar value : " << scrollbarValue ;
            
            double zoomFactor = (numDegrees.y() > 0) ? 1.15 : (1.0 / 1.15);

            double newSceneWidth = m_scene->width() * zoomFactor;

            double minWidth = m_view->viewport()->width(); // on va limiter le dézom pour qu'au minimum la scene fait la talle du viewport 
            double maxWidth = std::numeric_limits<int>::max() - 1000000.0; // en cas de vidéo très très longue le zoom peut causer des problèmes, -1 000 000 pour de la marge au cas ou
            
            int64_t duration = ProjectManager::instance().projet()->media->duration();
            double fps = ProjectManager::instance().projet()->media->fps();
            
            if (duration > 0 && fps > 0) {
                double frameMs = 1000.0 / fps;
                double totalFrames = static_cast<double>(duration) / frameMs;
                
                maxWidth = std::min( maxWidth, (totalFrames * m_minPxBetweenTicks)); 
                // la scene fera au maximum : nb de frames * l'espacement entre les ticks
                
                // si vidéo courte, le taille du view port peut être supérieur à maxWidth
                if (maxWidth < minWidth) maxWidth = minWidth;
            }
            //limite la taille avec les valeurs précédentes
            m_sceneWidth = std::clamp(newSceneWidth, minWidth, maxWidth);
            m_scene->setSceneRect(0, 0, m_sceneWidth, m_scene->height());


            m_ruler->setSize(m_sceneWidth, m_rulerHeight); // on met à jour la taille de la ruler pour fit la scene
            updateCursorPos(m_vlcTime); // on met à jour la position du curseur, apres un zoom/dezoom la position change
        }
        
    }
    return QWidget::eventFilter(watched, event);
}
