#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsItem>

class TimelineView : public QGraphicsView
{
Q_OBJECT 

public:
    explicit TimelineView(QGraphicsScene *scene, QWidget *parent = nullptr);

signals:
    void zoomRequested(double zoomFactor);

protected:
    void wheelEvent(QWheelEvent *event) override;
    
};

#endif