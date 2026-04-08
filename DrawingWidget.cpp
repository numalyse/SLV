#include "DrawingWidget.h"

#include "PrefManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QFrame>
#include <QMouseEvent>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    initDrawingSurface();
    initDrawingToolbar();
}

void DrawingWidget::initDrawingSurface(){
    m_drawingSurfaceW = new QWidget(this);
    m_drawingSurface = new QFrame(m_drawingSurfaceW);
    m_drawingSurface->setGeometry(0, 0, m_mediaRect.width(), m_mediaRect.height());
    m_drawingSurface->setStyleSheet("background: transparent;");
    m_drawingSurfaceW->hide();
}

void DrawingWidget::initDrawingToolbar(){
    m_drawingToolbar = new QWidget(this);
    containerBackground = new QFrame(m_drawingToolbar);
    containerBackground->setGeometry(50, m_mediaRect.height()-200-50, 50, 200);
    m_drawingToolbar->setStyleSheet(
        "QFrame {"
        " background-color: palette(base);"
        " border-radius: 6px;"
        " border: 1px solid palette(button);"
        "}"
        "QLabel {"
        " border: none;"
        "}");
    
    m_penModeBtn = new ToolbarToggleButton(
    this, 
    false,
    "auto_segmentation_white",
    PrefManager::instance().getText("deactivate") + " " + PrefManager::instance().getText("tooltip_drawing_mode"),
    "auto_segmentation",
    PrefManager::instance().getText("activate") + " " + PrefManager::instance().getText("tooltip_drawing_mode")
    );
    
    m_drawingToolbar->hide();
}

void DrawingWidget::showDrawingMode(bool isEnabled)
{
    m_isEnabled = isEnabled;
    if (m_isEnabled){
        initDrawingToolbar();
        m_drawingToolbar->show();
        m_drawingToolbar->raise();

        initDrawingSurface();
        m_drawingSurfaceW->show();
        m_drawingSurfaceW->raise();
    } else {
        m_drawingToolbar->hide();
        m_drawingSurfaceW->hide();
    }

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
    containerBackground->setGeometry(50, m_mediaRect.height()-200-50, 50, 200);
    m_drawingSurface->setGeometry(rect);
    update();
}


// Gestion des Events

void DrawingWidget::paintEvent(QPaintEvent *)
{
    if (!m_isEnabled) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    m_pen.setColor(Qt::red);
    p.setPen(m_pen);

    p.translate(m_mediaRect.topLeft());

    int w = m_mediaRect.width();
    int h = m_mediaRect.height();

    for (const QPainterPath &path : m_paths)
    {
        p.drawPath(path);
    }

}

void DrawingWidget::enterEvent(QEnterEvent *event)
{
    //activateWindow();
    setFocus(Qt::MouseFocusReason);
    //raise();
    QWidget::enterEvent(event);
    qDebug() << "ici";
}

void DrawingWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    qDebug() << "pressed";
    if (event->button() == Qt::LeftButton && m_isEnabled)
    {
        m_drawing = true;

        QPoint p = event->pos() - m_mediaRect.topLeft();

        QPainterPath newPath;
        newPath.moveTo(p);

        m_paths.append(newPath);
    }
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drawing && !m_paths.isEmpty())
    {
        QPoint p = event->pos() - m_mediaRect.topLeft();

        m_paths.last().lineTo(p);

        update();
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_drawing = false;
    }
}
