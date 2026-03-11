#include "TimelineView.h"

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
            
            
        } else {
            m_isDragging = true;
            double clickPosition = static_cast<double>(mapToScene(event->pos()).x());
            emit cursorPositionRequested(clickPosition);
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
    m_isDragging = false;
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor); 
    }
}
