#include "Timeline/Items/AnnotationItem.h"

#include "Timeline/Items/AnnotationHandleItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QPen>

AnnotationItem::AnnotationItem(const Annotation& annot, double width, QGraphicsItem *parent)
: QGraphicsItem(parent), m_annot{annot}, m_width{width}
{
    setZValue(2);
}

QRectF AnnotationItem::boundingRect() const
{
    return QRectF(0, s_topMargin, m_width, s_height);
}

void AnnotationItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    p->setBrush(QBrush(m_annot.color));

    p->drawRect(0, s_topMargin, m_width, s_height);
}

void AnnotationItem::setWidth(double newWidth)
{
    if (m_width == newWidth) return;
    prepareGeometryChange(); 
    m_width = newWidth;
    update();
}
