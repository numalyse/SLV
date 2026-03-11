#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QTimer>

class TimelineView : public QGraphicsView
{
Q_OBJECT 

public:
    explicit TimelineView(QGraphicsScene *scene, QWidget *parent = nullptr);

signals:
    void zoomRequested(double zoomFactor);
    void cursorPositionRequested(double);
    
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private: 
    bool m_isDragging = false;
};

#endif