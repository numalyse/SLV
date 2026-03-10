#include "TimelineView.h"

TimelineView::TimelineView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
}

void TimelineView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    double zoomFactor = (numDegrees.y() > 0) ? 1.15 : (1.0 / 1.15);

    emit zoomRequested(zoomFactor);
}

/* void TimelineView::mousePressEvent(QMouseEvent *event)
{
    emit 
} */
