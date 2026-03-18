#include "Timeline/Items/ABMarkerItem.h"

#include <QPainter>

ABMarkerItem::ABMarkerItem(int height, int64_t vlcTime, QGraphicsItem *parent) 
: QGraphicsItem(parent), m_height{height}, m_vlcTime{vlcTime}
{
    setZValue(4);
}

QRectF ABMarkerItem::boundingRect() const
{
    return QRectF(-3, 0, 6, m_height);
}

void ABMarkerItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, true);

    p->setPen(QPen(QColor(255,140,56), 2));
    p->drawLine(0, 0, 0, m_height);

    p->setBrush(QColor(255,140,56));
    p->drawPolygon(s_points, 5);
}

