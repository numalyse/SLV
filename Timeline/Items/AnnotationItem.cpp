#include "Timeline/Items/AnnotationItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QPen>

AnnotationItem::AnnotationItem(const Annotation& annot, double width, double height, double topMargin, QGraphicsItem *parent)
: QGraphicsItem(parent), m_annot{annot}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    setZValue(2);
}

QRectF AnnotationItem::boundingRect() const
{
    return QRectF(0, m_topMargin, m_width, m_height);
}

void AnnotationItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    p->setBrush(QBrush(m_annot.color));

    p->drawRect(0, m_topMargin, m_width, m_height);

    if (m_annot.note != "")
    {
        // draw start of text
    }
    
}

void AnnotationItem::setWidth(double newWidth)
{
    if (m_width == newWidth) return;
    prepareGeometryChange(); 
    m_width = newWidth;
    update();
}
