#include "ShotItem.h"

#include "ProjectManager.h"

#include <QPainter>

ShotItem::ShotItem(Shot shot, int height) : m_height{height}
{
    int m_height = height;
    setZValue(0);
}

QRectF ShotItem::boundingRect() const
{
    return QRectF(-3, 0, 6, m_height);
}

void ShotItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing);

    p->setPen(QPen(Qt::blue, 2));

    p->drawLine(0, 0, 0, m_height);
}
