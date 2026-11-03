#include "Timeline/Items/AnnotationItem.h"

#include "Timeline/Items/AnnotationHandleItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
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
    updateHandlePositions();
}

QRectF AnnotationItem::boundingRect() const
{
    if (m_widthMs > 0.0 || m_pixelsPerMs <= 0.0)
        return QRectF(0, s_topMargin, m_widthMs, s_height);

    return QRectF(0, s_topMargin, effectiveDisplayWidthMs(m_pixelsPerMs), s_height);
}

double AnnotationItem::effectiveDisplayWidthMs(double pixelsPerMs) const
{
    return std::max(m_zeroDurationDisplayWidthMs, s_minVisibleWidthPx / pixelsPerMs);
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
        : effectiveDisplayWidthMs(pixelsPerMs);
    p->drawRect(QRectF(0.0, s_topMargin, displayWidthMs, s_height));

    const double availableWidthPx = m_widthMs * pixelsPerMs - 2 * s_textMargin;
    if (availableWidthPx < 20.0) return; // size too small, prevent rendering text

    static const QFont s_textFont = []{ QFont f; f.setPointSize(10); return f; }();
    static const QFontMetrics s_textMetrics(s_textFont);
    static const QFont s_nameFont = []{ QFont f; f.setPointSize(10); f.setBold(true); return f; }();
    static const QFontMetrics s_nameMetrics(s_nameFont);

    const int availableWidthPxInt = static_cast<int>(availableWidthPx);
    if (availableWidthPxInt != m_elidedForWidth) { 
        const QString note = QString(m_annot.note).replace('\n', " ");
        const int separatorWidth = s_textMetrics.horizontalAdvance(" ");
        const int nameWidth = s_nameMetrics.horizontalAdvance(m_annot.name);
        if (nameWidth + separatorWidth > availableWidthPxInt) {
            m_elidedName = s_nameMetrics.elidedText(m_annot.name, Qt::ElideRight, availableWidthPxInt);
            m_elidedNote.clear();
        } else {
            m_elidedName = m_annot.name;
            const int noteWidth = availableWidthPxInt - nameWidth - separatorWidth;
            m_elidedNote = s_textMetrics.elidedText(note, Qt::ElideRight, noteWidth);
        }
        m_elidedForWidth = availableWidthPxInt;
    }

    if (m_elidedName.isEmpty() && m_elidedNote.isEmpty()) return;

    p->save();
    p->scale(1.0 / pixelsPerMs, 1.0); // cancel painter horizontal scale to draw text
    p->setPen(qGray(m_annot.color.rgb()) > 128 ? Qt::black : Qt::white);
    p->setFont(s_nameFont);
    p->drawText(QRectF(s_textMargin, s_topMargin, availableWidthPx, s_height), Qt::AlignVCenter | Qt::AlignLeft, m_elidedName);

    const double noteX = s_textMargin + s_nameMetrics.horizontalAdvance(m_elidedName)
        + s_textMetrics.horizontalAdvance(" ");
    const double noteWidth = availableWidthPx - (noteX - s_textMargin);
    if (!m_elidedNote.isEmpty() && noteWidth > 0.0) {
        p->setFont(s_textFont);
        p->drawText(QRectF(noteX, s_topMargin, noteWidth, s_height), Qt::AlignVCenter | Qt::AlignLeft, m_elidedNote);
    }
    p->restore();
}

void AnnotationItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setToolTip(m_annot.name.isEmpty() ? m_annot.note : "<b>" + m_annot.name + "</b><br>" + m_annot.note);
    updateHandleVisibility(event->pos());
    QGraphicsItem::hoverEnterEvent(event);
}

void AnnotationItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    updateHandleVisibility(event->pos());
    QGraphicsItem::hoverMoveEvent(event);
}

void AnnotationItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    m_leftHandle->setVisible(false);
    m_rightHandle->setVisible(false);
    unsetCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

void AnnotationItem::setWidth(double newWidthMs)
{
    if (m_widthMs == newWidthMs) return;
    prepareGeometryChange();
    m_widthMs = newWidthMs;
    updateHandlePositions();
    invalidateTextCache();
    update();
}

void AnnotationItem::setPixelsPerMs(double pixelsPerMs)
{
    if (m_pixelsPerMs == pixelsPerMs) return;
    prepareGeometryChange();
    m_pixelsPerMs = pixelsPerMs;
    updateHandlePositions();
    update();
}

void AnnotationItem::setZeroDurationDisplayWidthMs(double widthMs)
{
    if (m_zeroDurationDisplayWidthMs == widthMs) return;
    prepareGeometryChange();
    m_zeroDurationDisplayWidthMs = widthMs;
    updateHandlePositions();
    update();
}

void AnnotationItem::updateHandlePositions()
{
    const double handleWidthMs = m_widthMs > 0.0
        ? m_widthMs
        : effectiveDisplayWidthMs(m_pixelsPerMs);
    m_rightHandle->setX(handleWidthMs);
}

void AnnotationItem::updateHandleVisibility(const QPointF& mousePos)
{
    const double pixelsPerMs = m_pixelsPerMs;
    if (pixelsPerMs <= 0.0) {
        m_leftHandle->setVisible(false);
        m_rightHandle->setVisible(false);
        return;
    }

    const double mouseX = mousePos.x() * pixelsPerMs;
    const double widthPx = (m_widthMs > 0.0
        ? m_widthMs
        : effectiveDisplayWidthMs(pixelsPerMs)) * pixelsPerMs;
    m_leftHandle->setVisible(mouseX <= s_handleHoverMarginPx);
    m_rightHandle->setVisible(widthPx - mouseX <= s_handleHoverMarginPx);
    if (m_leftHandle->isVisible() || m_rightHandle->isVisible())
        setCursor(QCursor(Qt::SplitHCursor));
    else
        unsetCursor();
}

void AnnotationItem::setDraggingHandle(AnnotationHandleItem* handle)
{
    m_leftHandle->setVisible(handle == m_leftHandle);
    m_rightHandle->setVisible(handle == m_rightHandle);
}
