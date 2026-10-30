#include "Timeline/Items/AnnotationItem.h"

#include "Timeline/Items/AnnotationHandleItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QFontMetrics>
#include <algorithm>

AnnotationItem::AnnotationItem(const Annotation& annot, double widthMs, QGraphicsItem *parent)
: QGraphicsItem(parent), m_annot{annot}, m_widthMs{widthMs}
{
    setZValue(4);
    setAcceptHoverEvents(true);

    m_leftHandle = new AnnotationHandleItem(true, this);
    m_rightHandle = new AnnotationHandleItem(false, this);
    m_leftHandle->setPos(0, s_topMargin);
    m_rightHandle->setPos(m_widthMs, s_topMargin);
    m_leftHandle->setVisible(false);
    m_rightHandle->setVisible(false);
}

QRectF AnnotationItem::boundingRect() const
{
    if (m_widthMs > 0.0 || m_pixelsPerMs <= 0.0)
        return QRectF(0, s_topMargin, m_widthMs, s_height);

    const double halfWidthMs = s_minVisibleWidthPx / (2.0 * m_pixelsPerMs);
    return QRectF(-halfWidthMs, s_topMargin, 2.0 * halfWidthMs, s_height);
}

void AnnotationItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    // cosmetic pen, the horizontal scale will not thicken its borders
    QPen pen(Qt::black);
    pen.setCosmetic(true);
    p->setPen(pen);
    p->setBrush(QBrush(m_annot.color));

    // the layer transform will resize the item based on its transform
    // horizontal factor of the parent layer transform
    const double pixelsPerMs = p->transform().m11();
    if (pixelsPerMs <= 0.0) return;

    const double displayWidthMs = m_widthMs > 0.0
        ? m_widthMs
        : s_minVisibleWidthPx / pixelsPerMs;
    const double displayStartMs = m_widthMs > 0.0 ? 0.0 : -displayWidthMs / 2.0;
    p->drawRect(QRectF(displayStartMs, s_topMargin, displayWidthMs, s_height));

    const double availableWidthPx = m_widthMs * pixelsPerMs - 2 * s_textMargin;
    if (availableWidthPx < 20.0) return; // size too small, prevent rendering text

    static const QFont s_textFont = []{ QFont f; f.setPointSize(10); return f; }();
    static const QFontMetrics s_textMetrics(s_textFont);

    const int availableWidthPxInt = static_cast<int>(availableWidthPx);
    if (availableWidthPxInt != m_elidedForWidth) { 
        QString text = m_annot.name + " " + QString(m_annot.note).replace('\n', " ");
        m_elidedText = s_textMetrics.elidedText(text, Qt::ElideRight, availableWidthPxInt);
        m_elidedForWidth = availableWidthPxInt;
    }

    if (m_elidedText.isEmpty()) return;

    p->save();
    p->scale(1.0 / pixelsPerMs, 1.0); // cancel painter horizontal scale to draw text
    p->setFont(s_textFont);
    p->setPen(qGray(m_annot.color.rgb()) > 128 ? Qt::black : Qt::white);
    p->drawText(QRectF(s_textMargin, s_topMargin, availableWidthPx, s_height), Qt::AlignVCenter | Qt::AlignLeft, m_elidedText);
    p->restore();
}

void AnnotationItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    m_leftHandle->setVisible(true);
    m_rightHandle->setVisible(true);
    QGraphicsItem::hoverEnterEvent(event);
}

void AnnotationItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    m_leftHandle->setVisible(false);
    m_rightHandle->setVisible(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

void AnnotationItem::setWidth(double newWidthMs)
{
    if (m_widthMs == newWidthMs) return;
    prepareGeometryChange();
    m_widthMs = newWidthMs;
    m_rightHandle->setX(m_widthMs);
    invalidateTextCache();
    update();
}

void AnnotationItem::setPixelsPerMs(double pixelsPerMs)
{
    if (m_pixelsPerMs == pixelsPerMs) return;
    prepareGeometryChange();
    m_pixelsPerMs = pixelsPerMs;
    update();
}
