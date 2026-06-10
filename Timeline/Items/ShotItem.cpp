#include "ShotItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QPen>

ShotItem::ShotItem(Shot shot, double width, double height, double topMargin , QGraphicsItem* parent) 
: QGraphicsItem(parent), m_shot{shot}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    setZValue(0);

    // pour avoir le texte au dessus de l'audio visualiser
    m_selectionBox = new QGraphicsRectItem();
    m_selectionBox->setZValue(4); 

    m_selectionBox->setPen(QPen(Qt::white, 2)); 
    m_selectionBox->setBrush(Qt::NoBrush); 

    m_selectionText = new QGraphicsTextItem(m_selectionBox);
    QFont textFont; 
    textFont.setPointSize(16);
    textFont.setBold(true); 
    m_selectionText->setFont(textFont);

    m_selectionBox->setVisible(false);
}

ShotItem::~ShotItem()
{
    delete m_selectionBox;
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

    //QPen borderPen(m_selected ? Qt::white : m_shot.borderColor);
    QPen borderPen(m_shot.borderColor);
    //borderPen.setWidth(m_selected ? 2 : 1);
    borderPen.setWidth(1);
    p->setPen(borderPen);

    QColor drawColor = m_selected ? m_shot.color.lighter(150) : m_shot.color;
    p->setBrush(QBrush(drawColor));

    p->drawRect(0, m_topMargin, m_width, m_height);

    if(!m_pixmap.isNull() && m_width > s_minSizeForImage){

        double targetHeight = m_height - 4;
        double scaleRatio = targetHeight / m_pixmap.height();
        double scaledImgWidth = m_pixmap.width() * scaleRatio;
        double finalDrawWidth = qMin(scaledImgWidth, m_width);

        QRectF target(2.0, m_topMargin+2.0, finalDrawWidth, targetHeight);
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
    updateTextPosition();
    update();
}

void ShotItem::setSelected(bool state) { 

    if (!m_selectionBox) {
        qDebug() << "[SHOTITEM] setSelected : m_selectionBox null";
        return;
    }
    
    m_selected = state;

    if (m_selected) {
        if (this->scene() != nullptr && m_selectionBox->scene() != this->scene()) {
            this->scene()->addItem(m_selectionBox);
        }

        m_selectionText->setPlainText(QString::number(m_selectedNumber));
        m_selectionText->setDefaultTextColor(m_shot.borderColor);
        
        updateTextPosition();
        
        m_selectionBox->setVisible(true);
    } else {
        m_selectionBox->setVisible(false);
    }

    update();
}

void ShotItem::updateTextPosition(){
    m_selectionBox->setRect(0, 0, m_width, m_height);
    m_selectionBox->setPos(this->mapToScene(0, m_topMargin));

    QRectF textRect = m_selectionText->boundingRect();
    double xPos = (m_width - textRect.width()) / 2.0;
    double yPos = (m_height - textRect.height()) / 2.0;
    
    m_selectionText->setPos(xPos, yPos);
}