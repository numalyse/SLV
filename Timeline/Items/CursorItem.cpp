#include "CursorItem.h"

#include <QPainter>

CursorItem::CursorItem(int height, QGraphicsItem* parent) : QGraphicsItem(parent), m_height{height}
{
    setZValue(5);
}

QRectF CursorItem::boundingRect() const {
    return QRectF(-3, 0, 6, m_height);
}

void CursorItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, true);

    p->setPen(QPen(QColor(220, 220 , 220, 255), 2));
    p->drawLine(0, 0, 0, m_height);

    p->setBrush(QColor(220, 220 , 220, 255));
    p->drawPolygon(s_points, 5);


}


void CursorItem::setHeight(int height) {
    if (m_height == height) return;
    prepareGeometryChange(); 
    m_height = height;
    update();
}
