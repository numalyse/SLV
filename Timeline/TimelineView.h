#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QTimer>

class ABMarkerItem;
class AnnotationHandleItem;
class ShotItem;
class AudioShotItem;

class TimelineView : public QGraphicsView
{
Q_OBJECT 

public:
    explicit TimelineView(QGraphicsScene *scene, QWidget *parent = nullptr);

signals:
    void zoomRequested(double zoomFactor, int mouseX);
    void cursorPositionRequested(double);
    void shotSelectionRequested(ShotItem*, AudioShotItem*, const bool exclusive);
    void itemRightClick(QPoint, QGraphicsItem*);
    void isDragging(bool);
    void abMarkerDragged(ABMarkerItem*, const double);
    void annotationHandleDragged(AnnotationHandleItem*, const double);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private: 

    enum class DragMode { None, Pan, TimeCursor, ABMarker, AnnotationHandle };
    DragMode m_dragMode = DragMode::None;
    QGraphicsItem* m_draggedItem = nullptr;

    QPoint m_lastPanPos;
};

#endif
