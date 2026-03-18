#include "Timeline/TimelineView.h"

#include "Timeline/ItemTypes.h"

#include <QDebug>
#include <QScrollbar>

TimelineView::TimelineView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
}

void TimelineView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    double zoomFactor = (numDegrees.y() > 0) ? 1.15 : (1.0 / 1.15);

    int mouseX = event->position().x(); 

    emit zoomRequested(zoomFactor, mouseX);
}

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {   
        if (event->modifiers() & Qt::ControlModifier) {
            m_isPanning = true;
            m_lastPanPos = event->pos(); 
            setCursor(Qt::ClosedHandCursor);
            
        } else if (event->modifiers() & Qt::ShiftModifier) {
            
            if (QGraphicsItem *item = itemAt(event->pos())) {
                emit itemLeftClick(item);
            } 
        } else {
            m_isDragging = true;
            emit isDragging(true);
            double clickPosition = static_cast<double>(mapToScene(event->pos()).x());
            emit cursorPositionRequested(clickPosition);
        }
    }else if (event->button() == Qt::RightButton) {
        QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
        for (auto* item : itemsAtCursor){ 
            if(item->type() == SLV::TypeShotItem){ // si le curseur est devant un shot item, on va quand même pouvoir récuperer le shot item
                emit itemRightClick( event->globalPos() , item);
            }
        }
    }
}
void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isDragging){
        emit cursorPositionRequested(static_cast<double>(mapToScene(event->pos()).x()));
    }else if( m_isPanning){
        int deltaX = event->pos().x() - m_lastPanPos.x();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - deltaX);
        m_lastPanPos = event->pos();
    }

}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    emit isDragging(false);
    m_isDragging = false;
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor); 
    }
}
