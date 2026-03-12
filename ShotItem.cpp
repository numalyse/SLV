#include "ShotItem.h"

#include "ProjectManager.h"

#include <QPainter>

ShotItem::ShotItem(Shot shot, double width, double height, double topMargin , QGraphicsItem* parent) 
: QGraphicsItem(parent), m_shot{shot}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    setZValue(0);
}

QRectF ShotItem::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

void ShotItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    p->setBrush(QBrush(QColor(100, 150, 255, 150))); 

    p->drawRect(0, m_topMargin, m_width, m_height);
}

void ShotItem::setWidth(double width)
{
    if (m_width == width) return;
    prepareGeometryChange(); 
    m_width = width;
    update();
}
