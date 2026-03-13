#include "ShotItem.h"

#include "ProjectManager.h"

#include <QPainter>

ShotItem::ShotItem(Shot shot, double width, double height, double topMargin , QGraphicsItem* parent) 
: QGraphicsItem(parent), m_shot{shot}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    m_pixmap = QPixmap(":/icons/test.png");
    if (!shot.tagImage.isNull()) {
        m_pixmap = QPixmap(QPixmap::fromImage(shot.tagImage));
    }
    setZValue(0);
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

    p->setBrush(QBrush(m_shot.color)); 
    p->drawRect(0, m_topMargin, m_width, m_height);


    if(!m_pixmap.isNull() && m_width > s_minSizeForImage){

        QRectF target(0.0, m_topMargin, m_width, m_height/1.5);

        QSize targetImgSize(100, 30);
        
        QPixmap resizedPixmap = m_pixmap.scaled(
            targetImgSize,                      
            Qt::KeepAspectRatio,      
            Qt::FastTransformation          
        ); 

        QRectF srcRectf =  resizedPixmap.rect().toRectF();

        double imgWidth = srcRectf.width();

        if( m_width < imgWidth ){
            target.setWidth(m_width);

        }else if( m_width > srcRectf.width() ){
            target.setWidth(imgWidth);

        } 

        p->drawPixmap(target, resizedPixmap, srcRectf);

    }


}

void ShotItem::setWidth(double width)
{
    if (m_width == width) return;
    prepareGeometryChange(); 
    m_width = width;
    update();
}
