#include "TimelineView.h"

#include <QDebug>

TimelineView::TimelineView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
}

void TimelineView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    double zoomFactor = (numDegrees.y() > 0) ? 1.15 : (1.0 / 1.15);

    emit zoomRequested(zoomFactor);
}

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    m_isDragging = true;
    emit cursorPositionRequested(static_cast<double>(mapToScene(event->pos()).x()));
}
void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isDragging){
        emit cursorPositionRequested(static_cast<double>(mapToScene(event->pos()).x()));
    }

}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
}
