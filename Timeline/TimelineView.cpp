#include "Timeline/TimelineView.h"

#include "Timeline/ItemTypes.h"
#include "Timeline/Items/ABMarkerItem.h"
#include "Timeline/Items/AnnotationHandleItem.h"
#include "Timeline/Items/AudioShotItem.h"
#include "Timeline/Items/CursorItem.h"

#include <QDebug>
#include <QScrollBar>
#include <QToolTip>

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
        if (event->modifiers() & Qt::ShiftModifier) {
            m_dragMode = DragMode::Pan;
            m_lastPanPos = event->pos();
            setCursor(Qt::ClosedHandCursor);

        } else if (event->modifiers() & Qt::ControlModifier) {
            QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
            for (auto* item : itemsAtCursor){
                if (item->type() == SLV::TypeAudioShotItem){
                    emit shotSelectionRequested(nullptr, static_cast<AudioShotItem*>(item), false);
                } else if (item->type() == SLV::TypeShotItem){
                    emit shotSelectionRequested(static_cast<ShotItem*>(item), nullptr, false);
                }
            }

        } else { // click with no modifiers

            // priority set by z value, other items like audio visualizer are ignored
            for (auto* item : items(event->pos())) {
                switch (item->type())
                {
                case SLV::TypeABMarkerItem:
                    m_dragMode = DragMode::ABMarker;
                    m_draggedItem = item;
                    return;
                case SLV::TypeAnnotationHandleItem:
                    m_dragMode = DragMode::AnnotationHandle;
                    m_draggedItem = static_cast<AnnotationHandleItem*>(item)->annotParent()->closestHandle(mapToScene(event->pos()).x());
                    static_cast<AnnotationHandleItem*>(m_draggedItem)->annotParent()->setDraggingHandle(
                        static_cast<AnnotationHandleItem*>(m_draggedItem));
                    return;
                case SLV::TypeCursorItem:
                    m_dragMode = DragMode::TimeCursor;
                    m_draggedItem = item;
                    emit isDragging(true);
                    emit cursorPositionRequested(mapToScene(event->pos()).x());
                    return;
                case SLV::TypeRulerItem:
                    m_dragMode = DragMode::TimeCursor;
                    m_draggedItem = nullptr;
                    for (QGraphicsItem* sceneItem : scene()->items()) {
                        if (sceneItem->type() == SLV::TypeCursorItem) {
                            m_draggedItem = sceneItem;
                            break;
                        }
                    }
                    emit isDragging(true);
                    emit cursorPositionRequested(mapToScene(event->pos()).x());
                    return;
                case SLV::TypeShotItem:
                    emit shotSelectionRequested(static_cast<ShotItem*>(item), nullptr, true);
                    return;
                case SLV::TypeAudioShotItem:
                    emit shotSelectionRequested(nullptr, static_cast<AudioShotItem*>(item), true);
                    return;
                }
            }
        }
    }else if (event->button() == Qt::RightButton) {
        QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
        for (auto* item : itemsAtCursor) {
            // emit signal if cursor is on top of a shot item (even if underneath by other item)
            if (item->type() == SLV::TypeShotItem
                || item->type() == SLV::TypeAudioShotItem
                || item->type() == SLV::TypeAnnotationItem
                || item->type() == SLV::TypeABMarkerItem
                || item->type() == SLV::TypeCursorItem) {
                emit itemRightClick(event->globalPos(), item);
                break;
            } else if (item->type() == SLV::TypeAnnotationHandleItem) {
                auto* handle = static_cast<AnnotationHandleItem*>(item);
                emit itemRightClick(event->globalPos(), handle->annotParent());
                break;
            }
        }
    }
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    switch (m_dragMode)
    {
    case DragMode::Pan: {
        int deltaX = event->pos().x() - m_lastPanPos.x();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - deltaX);
        m_lastPanPos = event->pos();
        break;
    }
    
    case DragMode::TimeCursor:
        emit cursorPositionRequested(mapToScene(event->pos()).x());
        if (m_draggedItem && m_draggedItem->type() == SLV::TypeCursorItem) {
            const QString tooltip = static_cast<CursorItem*>(m_draggedItem)->toolTip();
            if (!tooltip.isEmpty())
                QToolTip::showText(event->globalPosition().toPoint(), tooltip, viewport());
        }
        break;

    case DragMode::ABMarker:
        emit abMarkerDragged(static_cast<ABMarkerItem*>(m_draggedItem), mapToScene(event->pos()).x());
        if (m_draggedItem) {
            const QString tooltip = m_draggedItem->toolTip();
            if (!tooltip.isEmpty())
                QToolTip::showText(event->globalPosition().toPoint(), tooltip, viewport());
        }
        break;

    case DragMode::AnnotationHandle:
        emit annotationHandleDragged(static_cast<AnnotationHandleItem*>(m_draggedItem), mapToScene(event->pos()).x());
        break;

    case DragMode::None:
        QGraphicsView::mouseMoveEvent(event);
        break;

    default:
        break;
    }
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragMode == DragMode::TimeCursor)
        emit isDragging(false);

    if (m_dragMode == DragMode::TimeCursor || m_dragMode == DragMode::ABMarker)
        QToolTip::hideText();

    if (m_dragMode == DragMode::AnnotationHandle && m_draggedItem) {
        auto* handle = static_cast<AnnotationHandleItem*>(m_draggedItem);
        handle->annotParent()->setDraggingHandle(nullptr);
    }
    
    setCursor(Qt::ArrowCursor);
    m_dragMode = DragMode::None;
    m_draggedItem = nullptr;
}
