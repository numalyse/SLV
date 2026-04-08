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

    m_palette = {
        QColor{229, 0, 0, 255},     // Rouge
        QColor{255, 141, 0, 255},   // Orange
        QColor{255, 238, 0, 255},   // Jaune
        QColor{0, 129, 33, 255},    // Vert
        QColor{0, 76, 255, 255},    // Bleu
        QColor{118, 1, 136, 255}    // Violet
    };

    initDrawingSurface();
    initDrawingToolbar();
}

void DrawingWidget::initDrawingSurface(){
    if (!m_drawingSurface) {
        m_drawingSurface = new QWidget(this);
        m_drawingSurface->setGeometry(m_mediaRect);
        //m_drawingSurface->setStyleSheet("background-color: rgba(255, 255, 255, 0.25);");
        m_drawingSurface->setStyleSheet("background-color: rgba(255, 255, 255, 0.01);");
        //m_drawingSurface->setStyleSheet("background: transparent;");
        m_drawingSurface->hide();
    }

    if (m_drawingCanvas.size() != m_drawingSurface->size()) {
        QImage newCanvas(m_drawingSurface->size(), QImage::Format_ARGB32_Premultiplied);
        newCanvas.fill(Qt::transparent);

        QPainter p(&newCanvas);
        p.drawImage(0, 0, m_drawingCanvas);

        m_drawingCanvas = newCanvas;
    }
}

QIcon DrawingWidget::genIconPreviewColor(QColor color){
    QPixmap pixmap(30, 30);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 30, 30);

    QIcon m_previewColor(pixmap);
    return m_previewColor;
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
    
    
    QVBoxLayout* drawingToolbarLayout = new QVBoxLayout(containerBackground);
    //drawingToolbarLayout->addWidget(m_drawingToolbar);

    // PALETTE CHOIX COULEUR
    QHBoxLayout* colorLayout = new QHBoxLayout;
    for (const QColor& color : m_palette) {
        ToolbarButton* colorBtn = new ToolbarButton(
            m_drawingToolbar,
            " ",
            PrefManager::instance().getText("tooltip")
        );
        colorBtn->setIcon(genIconPreviewColor(color));
        connect(colorBtn, &ToolbarButton::clicked, this, [this, color]() {
            setColor(color);
        });
        colorLayout->addWidget(colorBtn);
    }

    // BOUTON COULEUR
    m_colorToolBtn = new ToolbarToggleHoverButton(
    m_drawingToolbar, 
    colorLayout,
    false,
    "",
    PrefManager::instance().getText("tooltip_color_tool"),
    "",
    PrefManager::instance().getText("tooltip_color_tool")
    );
    connect(m_colorToolBtn, &ToolbarToggleHoverButton::clicked, this, &DrawingWidget::binRequested);
    drawingToolbarLayout->addWidget(m_colorToolBtn);

    
    m_colorToolBtn->setIcon(genIconPreviewColor(m_color));


    // BOUTON CRAYON
    m_pencilToolBtn = new ToolbarToggleButton(
    m_drawingToolbar, 
    false,
    "auto_segmentation_white",
    PrefManager::instance().getText("tooltip_pencil_tool") + " " + PrefManager::instance().getText("(activated)"),
    "auto_segmentation",
    PrefManager::instance().getText("tooltip_pencil_tool") + " " + PrefManager::instance().getText("(deactivated)")
    );
    connect(m_pencilToolBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
    drawingToolbarLayout->addWidget(m_pencilToolBtn);

    // BOUTON GOMME
    m_eraserToolBtn = new ToolbarToggleButton(
    m_drawingToolbar, 
    false,
    "auto_segmentation_white",
    PrefManager::instance().getText("tooltip_eraser_tool") + " " + PrefManager::instance().getText("(activated)"),
    "auto_segmentation",
    PrefManager::instance().getText("tooltip_eraser_tool") + " " + PrefManager::instance().getText("(deactivated)")
    );
    connect(m_eraserToolBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
    drawingToolbarLayout->addWidget(m_eraserToolBtn);

    // BOUTON SUPPRIMER TOUT
    m_binToolBtn = new ToolbarButton(
    m_drawingToolbar,
    "delete_white",
    PrefManager::instance().getText("tooltip_bin_tool")
    );
    connect(m_binToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::binRequested);
    drawingToolbarLayout->addWidget(m_binToolBtn);

    m_drawingToolbar->hide();
}

void DrawingWidget::binRequested(){
    // TO DO : QDialog - Voulez-vous effacer tout le contenu ?

}

void DrawingWidget::updateToolbarButtonsState(){
    ToolbarToggleButton* senderBtn = qobject_cast<ToolbarToggleButton*>(sender());
    if (senderBtn == m_pencilToolBtn) {
        if (m_pencilToolBtn->isChecked()) {
            m_eraserToolBtn->setChecked(false);
            m_eraserToolBtn->setButtonState(false);
            m_drawing = true;
        } else {
            m_drawing = false;
        }
        m_pencilToolBtn->setButtonState(m_pencilToolBtn->isChecked());
        
    } else if (senderBtn == m_eraserToolBtn) {
        if (m_eraserToolBtn->isChecked()) {
            m_pencilToolBtn->setChecked(false);
            m_pencilToolBtn->setButtonState(false);
        }
        m_eraserToolBtn->setButtonState(m_eraserToolBtn->isChecked());
        m_drawing = false;
    }
}


void DrawingWidget::showDrawingMode(bool isEnabled)
{
    m_isEnabled = isEnabled;

    if (m_isEnabled) {
        //initDrawingSurface();
        //initDrawingToolbar();
        m_drawingSurface->show();
        m_drawingSurface->raise();

        m_drawingToolbar->show();
        m_drawingToolbar->raise();
    } else {
        m_drawingSurface->hide();
        m_drawingToolbar->hide();
    }

    update();
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
    if(containerBackground)
        containerBackground->setGeometry(50, m_mediaRect.height()-200-50, 50, 200);
    if(m_drawingSurface)
        m_drawingSurface->setGeometry(m_mediaRect);
    //this->setGeometry(m_mediaRect);
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
        //m_drawing = true;

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
    // if (event->button() == Qt::LeftButton)
    // {
    //     m_drawing = false;
    // }
}