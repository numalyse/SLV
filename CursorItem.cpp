#include "CursorItem.h"

#include <QPainter>

CursorItem::CursorItem(int height) : m_height{height}
{
    setZValue(5);
}

QRectF CursorItem::boundingRect() const {
    return QRectF(-1, 0, 3, m_height);
}

void CursorItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing);

    p->setPen(QPen(Qt::red, 2));

    p->drawLine(0, 0, 0, m_height);

}


void CursorItem::setHeight(int height) {
    if (m_height == height) return;
    prepareGeometryChange(); 
    m_height = height;
    update();
}
