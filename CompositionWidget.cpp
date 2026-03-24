#include "CompositionWidget.h"
#include <QPainter>

CompositionWidget::CompositionWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

}

void CompositionWidget::setOverlayMode(OverlayMode mode)
{
    m_mode = mode;
    update(); // redraw
}

CompositionWidget::OverlayMode CompositionWidget::overlayMode() const
{
    return m_mode;
}

void CompositionWidget::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void CompositionWidget::setLineWidth(int width)
{
    m_lineWidth = width;
    update();
}

void CompositionWidget::paintEvent(QPaintEvent *)
{
    if (m_mode == None)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPen pen(m_color, m_lineWidth);
    p.setPen(pen);

    switch (m_mode) {
    case RuleOfThirds:
        drawRuleOfThirds(p);
        break;
    case CenterCross:
        drawCenterCross(p);
        break;
    case Diagonals:
        drawDiagonals(p);
        break;
    default:
        break;
    }
}

void CompositionWidget::drawRuleOfThirds(QPainter &p)
{
    int w = width();
    int h = height();

    // vertical
    p.drawLine(w / 3, 0, w / 3, h);
    p.drawLine(2 * w / 3, 0, 2 * w / 3, h);

    // horizontal
    p.drawLine(0, h / 3, w, h / 3);
    p.drawLine(0, 2 * h / 3, w, 2 * h / 3);
}

void CompositionWidget::drawCenterCross(QPainter &p)
{
    int w = width();
    int h = height();

    p.drawLine(w / 2, 0, w / 2, h);
    p.drawLine(0, h / 2, w, h / 2);
}

void CompositionWidget::drawDiagonals(QPainter &p)
{
    int w = width();
    int h = height();

    p.drawLine(0, 0, w, h);
    p.drawLine(w, 0, 0, h);
}