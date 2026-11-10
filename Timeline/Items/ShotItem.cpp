#include "ShotItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QCursor>
#include <QBuffer>
#include <QIODevice>
#include <QToolTip>
#include <QWidget>
#include <utility>

ShotItem::ShotItem(Shot shot, double width, QGraphicsItem* parent)
: QGraphicsItem(parent), m_shot{shot}, m_widthMs{width}, m_baseColor{m_shot.color}
{
    setZValue(0);
    setAcceptHoverEvents(true);

    // to keep the shotItem undearneath the audio visualizer but to have our "selected m_selectedNumber above the visualizer"
    // we create an item that will follow shotItem but with a different z value 
    m_selectionBox = new QGraphicsRectItem();     // it can't have this as parent, it would place it below the audio visualizer
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

void ShotItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    m_isHovered = true;
    m_toolTipWidget = event->widget();
    requestTagImageIfNeeded();
    QGraphicsItem::hoverEnterEvent(event);
}

void ShotItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    m_isHovered = false;
    m_toolTipWidget.clear();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ShotItem::setTagImage(const QPixmap& pixmap)
{
    m_tagImage = pixmap;
    m_tagImageRequested = true;
    updateToolTip();
}

void ShotItem::clearTagImage()
{
    m_tagImage = QPixmap();
    m_tagImageRequested = false;
    updateToolTip();
    requestTagImageIfNeeded();
}

void ShotItem::setTagImageRequestHandler(std::function<void()> handler)
{
    m_tagImageRequestHandler = std::move(handler);
}

void ShotItem::setHoverTooltipText(const QString& text)
{
    m_hoverTooltipTextValue = text;
    updateToolTip();
}

void ShotItem::requestTagImageIfNeeded()
{
    if (m_tagImageRequested)
        return;

    m_tagImageRequested = true;
    if (m_tagImageRequestHandler)
        m_tagImageRequestHandler();
}

void ShotItem::updateToolTip()
{
    if (m_hoverTooltipTextValue.isEmpty() && !m_tagImageRequestHandler)
        return;

    QString toolTip = m_hoverTooltipTextValue;
    if (!m_tagImage.isNull()) {
        QByteArray imageData;
        QBuffer buffer(&imageData);
        if (buffer.open(QIODevice::WriteOnly) && m_tagImage.save(&buffer, "PNG")) {
            const QString imageUrl = QStringLiteral("data:image/png;base64,%1")
                                         .arg(QString::fromLatin1(imageData.toBase64()));
            toolTip.prepend(QStringLiteral("<img src=\"%1\"><br>").arg(imageUrl));
        } else {
            qWarning() << "[ShotItem] Impossible d'encoder la tag image du tooltip";
        }
    }

    setToolTip(toolTip);
    if (m_isHovered)
        QToolTip::showText(QCursor::pos(), toolTip, m_toolTipWidget);
}


void ShotItem::setThumbnail(const QPixmap& pixmap){
    m_pixmap = pixmap;
    update();
}

QRectF ShotItem::boundingRect() const
{
    return QRectF(0, topMargin(), m_widthMs, height());
}

void ShotItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    QPen borderPen(m_shot.borderColor);
    borderPen.setCosmetic(true); // cosmetic pen, the horizontal scale will not thicken its borders
    p->setPen(m_showBorder ? borderPen : QPen(Qt::NoPen));
    QColor drawColor = m_selected ? m_shot.color.lighter(150) : m_shot.color;

    p->setBrush(QBrush(drawColor));
    p->drawRect(0, topMargin(), m_widthMs, height());

    if (!m_showThumbnail)
        return;

    // to cancel painter horizontal scale to draw pixmap
    const double sx = p->transform().m11();
    if (sx <= 0.0) return;
    const double widthPx = m_widthMs * sx;

    if(!m_pixmap.isNull() && widthPx > s_minSizeForImage){
        double targetHeight = height() - 4;
        double scaleRatio = targetHeight / m_pixmap.height();
        double scaledImgWidth = m_pixmap.width() * scaleRatio;
        double finalDrawWidth = qMin(scaledImgWidth, widthPx);

        QRectF target(2.0, topMargin()+2.0, finalDrawWidth, targetHeight);
        double sourceCropWidth = finalDrawWidth / scaleRatio;

        QRectF srcRect(0, 0, sourceCropWidth, m_pixmap.height());

        p->save();
        p->scale(1.0 / sx, 1.0); 
        p->drawPixmap(target, m_pixmap, srcRect);
        p->restore();
    }

}

void ShotItem::setWidth(double width)
{
    if (m_widthMs == width) return;
    prepareGeometryChange(); 
    m_widthMs = width;
    updateTextPosition();
    update();
}

void ShotItem::setShowThumbnail(bool show)
{
    if (m_showThumbnail == show)
        return;
    m_showThumbnail = show;
    update();
}

void ShotItem::setShowBorder(bool show)
{
    if (m_showBorder == show)
        return;
    m_showBorder = show;
    m_selectionBox->setVisible(m_showBorder && m_selected);
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
    // ms to scene pixel
    const double widthPx = m_widthMs * sceneTransform().m11();

    m_selectionBox->setRect(0, 0, widthPx, height());
    m_selectionBox->setPos(this->mapToScene(0, topMargin()));

    QRectF textRect = m_selectionText->boundingRect();
    double xPos = (widthPx - textRect.width()) / 2.0;
    double yPos = (height() - textRect.height()) / 2.0;

    m_selectionText->setPos(xPos, yPos);
}