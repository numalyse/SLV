#include "AnnotationHandleItem.h"

#include <QPainter>
#include <QPen>

AnnotationHandleItem::AnnotationHandleItem(bool isLeftHandle, QGraphicsItem * parent)
: QGraphicsItem(parent), m_isLeft{isLeftHandle}
{
    if (parent) setZValue(parent->zValue() - 1);
}

QRectF AnnotationHandleItem::boundingRect() const
{
    return QRectF(-5 * s_yScale, 0, 10 * s_yScale, s_height);
}

void AnnotationHandleItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, true);

    p->setBrush(Qt::white);
    p->drawPolygon(getPolygonPoints(), getPolygonPointCount());
}
