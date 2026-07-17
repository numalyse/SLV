#include "Timeline/Items/AnnotationItem.h"

#include "Timeline/Items/AnnotationHandleItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QPen>
#include <QFontMetrics>
#include <algorithm>

AnnotationItem::AnnotationItem(const Annotation& annot, double width, QGraphicsItem *parent)
: QGraphicsItem(parent), m_annot{annot}, m_width{width}
{
    setZValue(4);

    m_annotTxtItem = new QGraphicsTextItem(this);
    QFont textFont;
    textFont.setPointSize(10);
    m_annotTxtItem->setFont(textFont);

    m_leftHandle = new AnnotationHandleItem(true, this);
    m_rightHandle = new AnnotationHandleItem(false, this);
    m_leftHandle->setPos(0, s_topMargin);
    m_rightHandle->setPos(m_width, s_topMargin);

    updateTextItem();
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

    p->setPen(QPen(Qt::black));
    p->setBrush(QBrush(m_annot.color));

    p->drawRect(0, s_topMargin, m_width, s_height);
}

void AnnotationItem::setWidth(double newWidth)
{
    if (m_width == newWidth) return;
    prepareGeometryChange();
    m_width = newWidth;
    m_rightHandle->setX(m_width);
    updateTextItem();
    update();
}

void AnnotationItem::updateTextItem()
{
    // *2 to account for left and right margin
    const double availableWidth = std::max(0.0, m_width - 2 * s_textMargin);

    // replaces \n with " " and elide the text fo fit in 
    QFontMetrics metrics(m_annotTxtItem->font());
    const QString text = m_annot.name + " " + m_annot.note.replace('\n', " ");
    const QString elidedText = metrics.elidedText(text, Qt::ElideRight, static_cast<int>(availableWidth));
    m_annotTxtItem->setPlainText(elidedText);

    // dark text on light backgrounds, light text on dark backgrounds
    m_annotTxtItem->setDefaultTextColor(qGray(m_annot.color.rgb()) > 128 ? Qt::black : Qt::white);

    // centers text on y 
    const double textHeight = m_annotTxtItem->boundingRect().height();
    const double y = s_topMargin + (s_height - textHeight) / 2.0;
    m_annotTxtItem->setPos(s_textMargin, y);
}
