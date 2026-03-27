#include "ShotItem.h"

#include <QPainter>

ShotItem::ShotItem(Shot shot, double width, double height, double topMargin , QGraphicsItem* parent) 
: QGraphicsItem(parent), m_shot{shot}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    setZValue(0);
}

void ShotItem::setThumbnail(const QPixmap& pixmap){
    m_pixmap = pixmap;
    update();
}

QRectF ShotItem::boundingRect() const
{
    return QRectF(0, m_topMargin, m_width, m_height);
}

void ShotItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    p->setPen(Qt::black);
    p->setBrush(QBrush(m_shot.color)); 
    p->drawRect(0, m_topMargin, m_width, m_height);


    if(!m_pixmap.isNull() && m_width > s_minSizeForImage){

        double targetHeight = m_height / 1.5;
        double scaleRatio = targetHeight / m_pixmap.height();
        double scaledImgWidth = m_pixmap.width() * scaleRatio;
        double finalDrawWidth = qMin(scaledImgWidth, m_width);

        QRectF target(0.0, m_topMargin, finalDrawWidth, targetHeight);
        double sourceCropWidth = finalDrawWidth / scaleRatio;

        QRectF srcRect(0, 0, sourceCropWidth, m_pixmap.height());

        p->drawPixmap(target, m_pixmap, srcRect);
    }
}

void ShotItem::setWidth(double width)
{
    if (m_width == width) return;
    prepareGeometryChange(); 
    m_width = width;
    update();
}
