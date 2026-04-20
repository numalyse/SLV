#include "CompositionWidget.h"

#include <QPainter>
#include <QPainterPath>

CompositionWidget::CompositionWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);
    setWindowFlag(Qt::NoDropShadowWindowHint);

}

void CompositionWidget::setOverlayMode(OverlayMode mode, bool isVFlipped, bool isHFlipped)
{
    m_mode = mode;
    m_isVFlipped = isVFlipped; 
    m_isHFlipped = isHFlipped; 
    update(); // redraw
}

enum OverlayMode CompositionWidget::overlayMode() const
{
    return m_mode;
}

void CompositionWidget::setVerticalFlip(bool vf){
    m_isVFlipped = vf; 
    update();
}

void CompositionWidget::setHorizontalFlip(bool hf){
    m_isHFlipped = hf; 
    update();
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

void CompositionWidget::onMediaRectChanged(const QRect &rect)
{
    m_mediaRect = rect;
    qDebug() << "position : " << this->pos();
    qDebug() << "CompositionWidget m_mediaRect : " << m_mediaRect;
    update();
}

void CompositionWidget::paintEvent(QPaintEvent *)
{
    if (m_mode == OverlayMode::None)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPen pen(m_color, m_lineWidth);
    p.setPen(pen);

    p.translate(m_mediaRect.topLeft());

    switch (m_mode) {
    case OverlayMode::RuleOfThirds:
        drawRuleOfThirds(p);
        break;
    case OverlayMode::CenterCross:
        drawCenterCross(p);
        break;
    case OverlayMode::Diagonals:
        drawDiagonals(p);
        break;
    case OverlayMode::L_Shape:
        drawL_Shape(p);
        break;
    case OverlayMode::GoldenRatio:
        drawGoldenRatio(p);
        break;        
    default:
        break;
    }
}

void CompositionWidget::drawRuleOfThirds(QPainter &p)
{
    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    // vertical
    p.drawLine(w / 3, 0, w / 3, h);
    p.drawLine(2 * w / 3, 0, 2 * w / 3, h);

    // horizontal
    p.drawLine(0, h / 3, w, h / 3);
    p.drawLine(0, 2 * h / 3, w, 2 * h / 3);
}

void CompositionWidget::drawCenterCross(QPainter &p)
{
    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    p.drawLine(w / 2, 0, w / 2, h);
    p.drawLine(0, h / 2, w, h / 2);
}

void CompositionWidget::drawDiagonals(QPainter &p)
{
    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    if (!m_isVFlipped && m_isHFlipped) {
        p.drawLine(w, 0, 0, h);
    }
    else if (m_isVFlipped && !m_isHFlipped) {
        p.drawLine(0, h, w, 0);
    }
    else if (m_isVFlipped && m_isHFlipped) {
        p.drawLine(w, h, 0, 0);
    }
    else {
        p.drawLine(0, 0, w, h);
    }
    
}

void CompositionWidget::drawL_Shape(QPainter &p)
{
    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    if (!m_isVFlipped && m_isHFlipped) {
        p.drawLine(3*w/4, h/4, 3*w/4, 3*h/4);
        p.drawLine(w/4, 3*h/4, 3*w/4, 3*h/4);
    }
    else if (m_isVFlipped && !m_isHFlipped) {
        p.drawLine(w/4, h/4, w/4, 3*h/4);
        p.drawLine(w/4, h/4, 3*w/4, h/4);
    }
    else if (m_isVFlipped && m_isHFlipped) {
        p.drawLine(3*w/4, h/4, 3*w/4, 3*h/4);
        p.drawLine(w/4, h/4, 3*w/4, h/4);
    }
    else {
        p.drawLine(w/4, h/4, w/4, 3*h/4);
        p.drawLine(w/4, 3*h/4, 3*w/4, 3*h/4);
    }
}

void drawArc90(QPainter &painter, QPointF C, QPointF P1, QPointF P2) {
    double r = QLineF(C, P1).length();

    QRectF rect(C.x() - r, C.y() - r, 2*r, 2*r);

    double angleStart = qRadiansToDegrees(qAtan2(-(P1.y() - C.y()), P1.x() - C.x()));
    if(angleStart < 0) angleStart += 360;

    int startAngle = static_cast<int>(angleStart * 16);

    double angleEnd = qRadiansToDegrees(qAtan2(-(P2.y() - C.y()), P2.x() - C.x()));
    if(angleEnd < 0) angleEnd += 360;

    double diff = angleEnd - angleStart;

    int spanAngle;
    if(diff > 0 && diff <= 180) {
        spanAngle = 90 * 16;
    } else if(diff < 0 && diff >= -180) {
        spanAngle = -90 * 16;
    } else if(diff > 180) {
        spanAngle = -90 * 16;
    } else {
        spanAngle = 90 * 16;
    }

    painter.drawArc(rect, startAngle, spanAngle);
}

QPointF rotate90(const QPointF &p, const QPointF &c, bool cw) {
    double dx = p.x() - c.x();
    double dy = p.y() - c.y();

    if(cw)
        return QPointF(c.x() + dy, c.y() - dx);
    else
        return QPointF(c.x() - dy, c.y() + dx);
}

QPointF normalize(const QPointF &v) {
    double len = std::sqrt(v.x()*v.x() + v.y()*v.y());
    if(len == 0) return QPointF(0,0);
    return QPointF(v.x()/len, v.y()/len);
}

void drawNCarresArcs(QPainter &p, int n, int w, int h) {
    double phi = 0.618;
    double size = h;

    QPointF C(size, size);
    QPointF P1(0, size);
    QPointF P2(size, 0);

    for(int i = 0; i < n; ++i) {
        drawArc90(p, C, P1, P2);

        qDebug() << "arc n°" << i << " avec :";
        qDebug() << "C  : " << C;
        qDebug() << "P1 : " << P1;
        qDebug() << "P2 : " << P2;

        P1 = P2;

        size *= phi;

        QPointF Cnext = rotate90(C, P2, true);
        QPointF dir = normalize(Cnext - P2);
        Cnext = P2 + dir * size;

        QPointF P2next = rotate90(P1, Cnext, true);

        C = Cnext;
        P2 = P2next;

        if(size < 1) break;
    }
}

void CompositionWidget::drawGoldenRatio(QPainter &p)
{
    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    qDebug() << "Rectangle de dessin : " << m_mediaRect;
    qDebug() << "w : " << w;
    qDebug() << "h : " << h;

    int n = 5;
    drawNCarresArcs(p, n, w, h);


}
