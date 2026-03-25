#include "CompositionWidget.h"
#include "PlayerWidget.h"

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
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
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

void CompositionWidget::onMediaSizeChanged(const QSize &size)
{
    m_mediaSize = size;
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
    case OverlayMode::S_Curve:
        drawS_Curve(p);
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

void CompositionWidget::drawS_Curve(QPainter &p)
{
    int w = width();
    int h = height();

    QPainterPath path;

    path.moveTo(0, h * 0.2);

    path.cubicTo(
        w * 0.25, 0,        
        w * 0.25, h,       
        w * 0.5, h * 0.5    
    );

    path.cubicTo(
        w * 0.75, 0,        
        w * 0.75, h,        
        w, h * 0.8          
    );

    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawPath(path);
}

void CompositionWidget::drawGoldenRatio(QPainter &p)
{
    int w = width();
    int h = height();
    //qDebug() << "w : " << width() << " h : " << height();

    double size = std::min(w, h);

    double x = (w - size) / 2.0;
    double y = (h - size) / 2.0;

    QRectF rect(x, y, size, size);

    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;

    // 👉 EXACTEMENT 8 arcs
    for (int i = 0; i < 8; ++i)
    {
        // 🔹 angles corrects (sens anti-horaire Qt)
        int startAngle = 0;
        switch (i % 4)
        {
        case 0: startAngle = 0; break;
        case 1: startAngle = 90; break;
        case 2: startAngle = 180; break;
        case 3: startAngle = 270; break;
        }

        p.drawArc(rect, startAngle * 16, 90 * 16);

        // 🔹 réduction selon le golden ratio
        double newSize = rect.width() / phi;

        QRectF next;

        switch (i % 4)
        {
        case 0: // coupe droite
            next = QRectF(rect.left(), rect.top(), newSize, rect.height());
            break;

        case 1: // coupe bas
            next = QRectF(rect.left(), rect.top(), rect.width(), newSize);
            break;

        case 2: // coupe gauche
            next = QRectF(rect.right() - newSize, rect.top(), newSize, rect.height());
            break;

        case 3: // coupe haut
            next = QRectF(rect.left(), rect.bottom() - newSize, rect.width(), newSize);
            break;
        }

        rect = next;
    }
}