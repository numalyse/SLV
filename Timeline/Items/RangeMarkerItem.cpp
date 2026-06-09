#include "Timeline/Items/RangeMarkerItem.h"

#include <QPainter>
#include <QCursor>

RangeMarkerItem::RangeMarkerItem(int height, int64_t vlcTime, QGraphicsItem *parent) 
: QGraphicsItem(parent), m_height{height}, m_vlcTime{vlcTime}
{
    setZValue(10);
    setAcceptHoverEvents(true);
}

QRectF RangeMarkerItem::boundingRect() const
{
    return QRectF(-4.5, 0, 10, m_height);
}

void RangeMarkerItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, true);

    p->setPen(QPen(m_color, 2));
    p->drawLine(0, 0, 0, m_height);

    p->setBrush(m_color);
    p->drawPolygon(getPolygonPoints(), getPolygonPointCount());
}

void RangeMarkerItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::PointingHandCursor);
}

void RangeMarkerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    unsetCursor();
}
