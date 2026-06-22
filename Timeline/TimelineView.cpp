#include "Timeline/TimelineView.h"

#include "Timeline/ItemTypes.h"

#include <QDebug>
#include <QScrollBar>

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
            m_isPanning = true;
            m_lastPanPos = event->pos(); 
            setCursor(Qt::ClosedHandCursor);
            
        } else if (event->modifiers() & Qt::ControlModifier) {
            QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
            for (auto* item : itemsAtCursor){
                if (item->type() == SLV::TypeAudioShotItem || item->type() == SLV::TypeShotItem){
                    emit itemShiftLeftClick(item);
                }
            }

        } else {
            QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
            QGraphicsItem* shotItem = nullptr;
            QGraphicsItem* audioShotItem = nullptr;
            for (auto* item : itemsAtCursor){
                if( item->type() == SLV::TypeABMarkerItem ){ // si le curseur est devant un shot item, on va quand même pouvoir récuperer le shot item
                    setCursor(Qt::ClosedHandCursor);
                    m_draggedABMarker = item;
                }
                if( item->type() == SLV::TypeCursorItem || item->type() == SLV::TypeRulerItem ){
                    m_draggedCursor = item;
                }

                if(item->type() == SLV::TypeShotItem){
                    shotItem = item;
                }
                if(item->type() == SLV::TypeAudioShotItem){
                    audioShotItem = item;
                }

            }
            if(m_draggedABMarker) return;

            if(m_draggedCursor){
                m_isDragging = true;
                emit isDragging(true);
                double clickPosition = static_cast<double>(mapToScene(event->pos()).x());
                emit cursorPositionRequested(clickPosition);
                return;
            } // le click sur le curseur prend le dessus sur le reste

            if(shotItem ){
                emit itemLeftClick(shotItem);
                return;
            }
            if(audioShotItem){
                emit itemLeftClick(audioShotItem);
                return;
            }
   
        }
    }else if (event->button() == Qt::RightButton) {
        QList<QGraphicsItem *> itemsAtCursor = items(event->pos());
        for (auto* item : itemsAtCursor){ 
            if(item->type() == SLV::TypeShotItem || item->type() == SLV::TypeAudioShotItem){ // si le curseur est devant un shot item, on va quand même pouvoir récuperer le shot item
                emit itemRightClick( event->globalPos() , item);
                break;
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
    if(m_draggedABMarker){
        emit abMarkerDragged(m_draggedABMarker, mapToScene(event->pos()).x());
    }
    else{
        unsetCursor();
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
    m_draggedABMarker = nullptr;
    m_draggedCursor = nullptr;
}
