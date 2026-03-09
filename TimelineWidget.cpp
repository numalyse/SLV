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
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setFixedHeight(m_sceneHeight);
    m_view->viewport()->installEventFilter(this);
    layout->addWidget(m_view); 


    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_cursorPosition);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

    m_view->fitInView( m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);

}

void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); 

    if (m_scene) 
    {
        m_scene->setSceneRect(0, 0, m_sceneWidth, event->size().height());
    }
    if(m_view)
    {
        m_view->fitInView( m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
    if (m_ruler)
    {
        m_ruler->setSize(m_sceneWidth, m_rulerHeight);
    }
    if(m_cursor)
    {
        m_cursor->setHeight(event->size().height());
    }
    
}


void TimelineWidget::updateCursorPos(int64_t vlcTime){
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
            qDebug() << " mouse wheele detecté ";
            QPoint mousePos = QCursor::pos(); 
            if ( ! m_view->rect().contains(m_view->mapFromGlobal(mousePos))) {
                qDebug() << "souris pas dans la view";
                return QWidget::eventFilter(watched, event); // si la souris est pas dans la view, pas de scroll
            }

            m_currentScale = m_view->transform().m11();

            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            QPoint numDegrees = wheelEvent->angleDelta() / 8;
            int scrollVertical = numDegrees.y();


            double factor{};
            if( scrollVertical > 0 ){
                factor = 1.15;
            }else {
                factor = 1 / 1.15;
            }
            m_view->scale(factor, 1);
        }
        
    }
    return QWidget::eventFilter(watched, event);
}
