#include "TimelineWidget.h"

#include "ProjectManager.h"
#include "RulerItem.h"
#include "CursorItem.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>

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

    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(100);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

}

void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); 

    int realViewportHeight = m_view->viewport()->height();

    if (m_scene) {
        m_scene->setSceneRect(0, 0, m_sceneWidth, realViewportHeight);
    }
    if (m_cursor) {
        m_cursor->setHeight(realViewportHeight);
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

            m_currentScale = m_view->transform().m11();

            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            QPoint numDegrees = wheelEvent->angleDelta() / 8;
            
            double factor{};
            numDegrees.y() > 0 ? factor=1.15 : factor = 1 / 1.15 ;

            double newSceneWidth = m_scene->width() * factor;

            if(newSceneWidth < m_view->viewport()->width()){
                m_sceneWidth = m_view->viewport()->width();
                m_scene->setSceneRect(0,0, m_sceneWidth, m_scene->height());
                m_ruler->setSize(m_view->viewport()->width(), m_rulerHeight);
            }else {
                m_sceneWidth = newSceneWidth > std::numeric_limits<int>().max() ? std::numeric_limits<int>().max() : newSceneWidth;
                m_scene->setSceneRect(0,0, m_sceneWidth, m_scene->height());
                m_ruler->setSize(m_sceneWidth, m_rulerHeight);
            }
            updateCursorPos(m_vlcTime);
            return true;

        }
        
    }
    return QWidget::eventFilter(watched, event);
}
