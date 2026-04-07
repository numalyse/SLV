#include "DrawingWidget.h"

#include <QPainter>
#include <QPainterPath>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}
void DrawingWidget::showDrawingMode(bool isEnabled)
{
    m_isEnabled = isEnabled;
    update(); // redraw
}

void DrawingWidget::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void DrawingWidget::setLineWidth(int width)
{
    m_lineWidth = width;
    update();
}

void DrawingWidget::onMediaRectChanged(const QRect &rect)
{
    m_mediaRect = rect;
    update();
}

void DrawingWidget::paintEvent(QPaintEvent *)
{
    if (!m_isEnabled) return;
    
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    setColor(Qt::red);
    QPen pen(m_color, m_lineWidth);
    p.setPen(pen);

    p.translate(m_mediaRect.topLeft());

    int w = m_mediaRect.width();
    int h = m_mediaRect.height();
    p.drawLine(w / 3, 0, w / 3, h);

}
