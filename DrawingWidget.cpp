#include "DrawingWidget.h"

#include "PrefManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QTransform>
#include <QHBoxLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QRadioButton>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background-color: rgba(0,0,0,0)");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_pen = QPen(m_color, m_lineWidth);

    m_palette = {
        {PrefManager::instance().getText("white"), QColor{255, 255, 255, 255}}, // Blanc
        {PrefManager::instance().getText("black"), QColor{0, 0, 0, 255}},       // Noir
        {PrefManager::instance().getText("red"), QColor{229, 0, 0, 255}},       // Rouge
        {PrefManager::instance().getText("orange"), QColor{255, 141, 0, 255}},  // Orange
        {PrefManager::instance().getText("yellow"), QColor{255, 238, 0, 255}},  // Jaune
        {PrefManager::instance().getText("green"), QColor{0, 129, 33, 255}},    // Vert
        {PrefManager::instance().getText("blue"), QColor{0, 76, 255, 255}},     // Bleu
        {PrefManager::instance().getText("purple"), QColor{118, 1, 136, 255}},  // Violet
    };

    initDrawingSurface();
    initDrawingToolbar();
}

void DrawingWidget::showDrawingMode(bool isEnabled)
{
    m_isEnabled = isEnabled;

    if (m_isEnabled) {
        m_drawingSurface->show();
        m_drawingSurface->raise();

        m_drawingToolbar->show();
        m_drawingToolbar->raise();

        if (!m_pencilToolBtn->isChecked())
            emit m_pencilToolBtn->click();
    } else {
        m_drawingSurface->hide();
        m_drawingToolbar->hide();
    }

    update();
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
        newCanvas.fill(Qt::red);

        QPainter p(&newCanvas);
        p.drawImage(0, 0, m_drawingCanvas);

        m_drawingCanvas = newCanvas;
    }
}

QIcon DrawingWidget::genIconPreviewColor(QColor color, int sizePen){
    QPixmap pixmap(30, 30);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);

    int x = (pixmap.width() - sizePen) / 2;
    int y = (pixmap.height() - sizePen) / 2;
    painter.drawEllipse(x, y, sizePen, sizePen);

    QIcon m_previewColor(pixmap);
    return m_previewColor;
}

void DrawingWidget::updatePen(){
    m_pen.setColor(m_color);
    m_pen.setWidth(m_lineWidth);
}

void DrawingWidget::initDrawingToolbar(){
    m_drawingToolbar = new QWidget(this);
    m_drawingToolbar->setContentsMargins(0,0,0,0);
    containerBackground = new QFrame(m_drawingToolbar);
    containerBackground->move(20, m_mediaRect.height()-m_drawingToolbar->height()-20);
    containerBackground->setContentsMargins(0,0,0,0);
    //containerBackground->setGeometry(50, m_mediaRect.height()-200-50, 50, 200);
    m_drawingToolbar->setStyleSheet(
        "QFrame {"
        " background-color: palette(base);"
        " border-radius: 6px;"
        " border: 1px solid palette(button);"
        "}"
        "QLabel {"
        " border: none;"
        "}"
    );
    
    
    QVBoxLayout* drawingToolbarLayout = new QVBoxLayout(containerBackground);
    //drawingToolbarLayout->addWidget(m_drawingToolbar);

    // BOUTON COULEUR
    QHBoxLayout* colorLayout = new QHBoxLayout;
    m_colorToolBtn = new ToolbarToggleHoverButton(
    m_drawingToolbar, 
    colorLayout,
    false,
    "",
    PrefManager::instance().getText("tooltip_color_tool"),
    "",
    PrefManager::instance().getText("tooltip_color_tool")
    );
    drawingToolbarLayout->addWidget(m_colorToolBtn);
    m_colorToolBtn->setIcon(genIconPreviewColor(m_color));

    // CHOIX PALETTE COULEUR
    for (const auto& [colorName, color] : m_palette) {
        ToolbarButton* colorBtn = new ToolbarButton(
            m_drawingToolbar,
            " ",
            colorName
        );
        colorBtn->setIcon(genIconPreviewColor(color));
        connect(colorBtn, &ToolbarButton::clicked, this, [this, color]() {
            setColor(color);
            m_colorToolBtn->setIcon(genIconPreviewColor(color));
            updatePen();
        });    
        connect(colorBtn, &ToolbarButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
        colorLayout->addWidget(colorBtn);
    }

    // BOUTON CRAYON
    QHBoxLayout* pencilLayout = new QHBoxLayout;
    m_pencilToolBtn = new ToolbarToggleHoverButton(
    m_drawingToolbar, 
    pencilLayout,
    false,
    "pencil_white",
    PrefManager::instance().getText("tooltip_pencil_tool") + " " + PrefManager::instance().getText("(activated)"),
    "pencil",
    PrefManager::instance().getText("tooltip_pencil_tool") + " " + PrefManager::instance().getText("(deactivated)")
    );
    connect(m_pencilToolBtn, &ToolbarToggleHoverButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
    drawingToolbarLayout->addWidget(m_pencilToolBtn);

    // CHOIX EPAISSEUR
    for(auto lineWidth : m_lineWidthLevels){
        ToolbarToggleButton* lineWidthBtn = new ToolbarToggleButton(
            m_drawingToolbar,
            false,
            " ",
            PrefManager::instance().getText("tooltip_linewidth") + " " + QString::number(lineWidth),
            " ",
            PrefManager::instance().getText("tooltip_linewidth") + " " + QString::number(lineWidth)
        );
        lineWidthBtn->setIcon(genIconPreviewColor(Qt::white, lineWidth*2));
        connect(lineWidthBtn, &ToolbarToggleButton::clicked, this, [this, lineWidth]() {
            setLineWidth(lineWidth);
            updatePen();
        });
        connect(lineWidthBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
        pencilLayout->addWidget(lineWidthBtn);
    }

    QFrame* lineSeparator = new QFrame();
    lineSeparator->setFrameShape(QFrame::VLine);
    pencilLayout->addWidget(lineSeparator);

    // CHOIX OPACITE
    for(auto opacity : m_opacityLevels){
        ToolbarToggleButton* opacityBtn = new ToolbarToggleButton(
            m_drawingToolbar,
            false,
            " ",
            PrefManager::instance().getText("tooltip_opacity") + " " + QString::number(opacity),
            " ",
            PrefManager::instance().getText("tooltip_opacity") + " " + QString::number(opacity)
        );
        QColor previewColor = Qt::white;
        previewColor.setAlphaF(opacity);

        opacityBtn->setIcon(genIconPreviewColor(previewColor));
        connect(opacityBtn, &ToolbarToggleButton::clicked, this, [this, opacity]() {
            setOpacity(opacity);
            setColor(m_color);
            updatePen();
        });
        connect(opacityBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
        pencilLayout->addWidget(opacityBtn);
    }

    // BOUTON GOMME
    m_eraserToolBtn = new ToolbarToggleButton(
    m_drawingToolbar,
    false,
    "eraser_white",
    PrefManager::instance().getText("tooltip_eraser_tool") + " " + PrefManager::instance().getText("(activated)"),
    "eraser",
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

    // BOUTON ANNULER
    ToolbarButton* m_undoToolBtn = new ToolbarButton(
        m_drawingToolbar,
        "undo_white",
        PrefManager::instance().getText("tooltip_undo_tool")
    );
    connect(m_undoToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::undoDrawing);
    drawingToolbarLayout->addWidget(m_undoToolBtn);

    // BOUTON RETABLIR
    ToolbarButton* m_redoToolBtn = new ToolbarButton(
        m_drawingToolbar,
        "redo_white",
        PrefManager::instance().getText("tooltip_redo_tool")
    );
    connect(m_redoToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::redoDrawing);
    drawingToolbarLayout->addWidget(m_redoToolBtn);

}

void DrawingWidget::binRequested(){
    if (m_paths.isEmpty())
        return;

    m_lastClearedPaths = m_paths;
    m_paths.clear();
    m_redoPathlist.clear();
    update();
}

void DrawingWidget::updateToolbarButtonsState(){
    ToolbarToggleButton* senderBtn = qobject_cast<ToolbarToggleButton*>(sender());
    //qDebug() << "updateToolbarButtonsState appelé par : " << senderBtn;
    if (senderBtn == m_pencilToolBtn) {
        if (m_pencilToolBtn->isChecked()) {
            m_eraserToolBtn->setChecked(false);
            m_eraserToolBtn->setButtonState(false);
            m_drawing = true;
            m_erasing = false;
        } else {
            m_drawing = false;
        }
        m_pencilToolBtn->setButtonState(m_pencilToolBtn->isChecked());
        
    } else if (senderBtn == m_eraserToolBtn) {
        if (m_eraserToolBtn->isChecked()) {
            m_pencilToolBtn->setChecked(false);
            m_pencilToolBtn->setButtonState(false);
            m_erasing = true;
            m_drawing = false;
        } else {
            m_erasing = false;
        }
        m_eraserToolBtn->setButtonState(m_eraserToolBtn->isChecked());
    } else {
        // C'est un bouton de réglage (couleur, épaisseur, opacité), on veut que m_drawing soit activé
        if (!m_drawing) {
            m_pencilToolBtn->setChecked(true);
            m_pencilToolBtn->setButtonState(true);
            m_eraserToolBtn->setChecked(false);
            m_eraserToolBtn->setButtonState(false);
            m_drawing = true;
            m_erasing = false;

            // On veut utliser setToggledIconFrame(true) sur les boutons opacityBtn quel réglage est actif par ces boutons

        }
    }
}

QPixmap DrawingWidget::eraseColor(){
    QPixmap brush(8, 8);
    brush.fill(Qt::white);
    
    QPainter p(&brush);
    p.fillRect(0, 0, 4, 4, Qt::gray);
    p.fillRect(4, 4, 4, 4, Qt::gray);
    p.end();
    return brush;
}

void DrawingWidget::setColor(const QColor &color)
{
    QColor colorWithOpacity = color;
    colorWithOpacity.setAlphaF(m_opacity);
    m_color = colorWithOpacity;

    update();
}

void DrawingWidget::setLineWidth(int width)
{
    m_lineWidth = width;
    update();
}

void DrawingWidget::setOpacity(float opacity)
{
    m_opacity = opacity;
    update();
}

// Gestion de l'historique des dessins

void DrawingWidget::undoDrawing()
{
    if (!m_lastClearedPaths.isEmpty()) {
        m_paths = m_lastClearedPaths;
        m_lastClearedPaths.clear();
        update();
        return;
    }

    if (!m_undoPathlist.isEmpty()) {
        m_paths.append(m_undoPathlist.takeLast());
        update();
        return;
    }

    if (!m_paths.isEmpty()) {
        m_redoPathlist.append(m_paths.takeLast());
        update();
    }
}

void DrawingWidget::redoDrawing()
{
    if (!m_redoPathlist.isEmpty()) {
        m_paths.append(m_redoPathlist.takeLast());
        update();
    }
}

// Gestion taille et position du widget

void DrawingWidget::scaleStrokeList(QVector<DrawingStroke> &strokes, double scaleX, double scaleY)
{
    if (strokes.isEmpty() || qFuzzyCompare(scaleX, double(1.0)) && qFuzzyCompare(scaleY, double(1.0)))
        return;

    QTransform transform;
    transform.scale(scaleX, scaleY);

    double widthFactor = (scaleX + scaleY) / 2.0;
    for (DrawingStroke &stroke : strokes) {
        stroke.path = transform.map(stroke.path);
        stroke.lineWidth = qMax(1, qRound(stroke.lineWidth * widthFactor));
    }
}

void DrawingWidget::scaleCurrentEraserPath(double scaleX, double scaleY)
{
    if (m_currentEraserPath.isEmpty() || (qFuzzyCompare(scaleX, double(1.0)) && qFuzzyCompare(scaleY, double(1.0))))
        return;

    QTransform transform;
    transform.scale(scaleX, scaleY);
    m_currentEraserPath = transform.map(m_currentEraserPath);
}

void DrawingWidget::onMediaRectChanged(const QRect &rect)
{
    if (m_mediaRect.width() > 0 && m_mediaRect.height() > 0 && m_mediaRect.size() != rect.size()) {
        double scaleX = double(rect.width()) / double(m_mediaRect.width());
        double scaleY = double(rect.height()) / double(m_mediaRect.height());

        scaleStrokeList(m_paths, scaleX, scaleY);
        scaleStrokeList(m_undoPathlist, scaleX, scaleY);
        scaleStrokeList(m_redoPathlist, scaleX, scaleY);
        scaleStrokeList(m_lastClearedPaths, scaleX, scaleY);
        scaleCurrentEraserPath(scaleX, scaleY);
    }

    m_mediaRect = rect;
    if(containerBackground)
        containerBackground->setGeometry(50, m_mediaRect.height()-200-50, 50, 200);
    if(m_drawingSurface)
        m_drawingSurface->setGeometry(m_mediaRect);
    update();
}


// Gestion des Events

void DrawingWidget::paintEvent(QPaintEvent *)
{
    if (!m_isEnabled) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(m_mediaRect.topLeft());

    // Dessin
    for (const DrawingStroke &stroke : m_paths)
    {
        QPen strokePen(stroke.color, stroke.lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(strokePen);
        p.drawPath(stroke.path);
    }
    

    // Gomme
    //if (m_erasing && !m_currentEraserPath.isEmpty())
    if (m_erasing)
    {
        //QPen eraser(Qt::white, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QPen eraser;
        QBrush eraserBrush(m_eraseBrush);
        eraserBrush.setStyle(Qt::TexturePattern);
        eraser.setBrush(eraserBrush);
        eraser.setWidth(m_eraserLineWidth);
        //eraser.setColor(QColor(255, 255, 255, 255));
        eraser.setStyle(Qt::SolidLine);
        eraser.setCapStyle(Qt::RoundCap);
        eraser.setJoinStyle(Qt::RoundJoin);

        p.setPen(eraser);
        p.drawPath(m_currentEraserPath);
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

    if (event->button() != Qt::LeftButton || !m_isEnabled)
        return;

    QPoint p = event->pos() - m_mediaRect.topLeft();

    if (m_erasing)
    {
        m_lastClearedPaths.clear();
        m_currentEraserPath = QPainterPath();
        m_currentEraserPath.moveTo(p);

        update();
        return;
    }

    if (m_drawing)
    {
        m_lastClearedPaths.clear();
        DrawingStroke newStroke;
        newStroke.path.moveTo(p);
        newStroke.color = m_color;
        newStroke.lineWidth = m_lineWidth;

        m_paths.append(newStroke);
    }

}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos() - m_mediaRect.topLeft();

    if (m_erasing)
    {
        m_currentEraserPath.lineTo(p);

        QPainterPathStroker eraserStroker;
        eraserStroker.setWidth(m_eraserLineWidth);
        eraserStroker.setCapStyle(Qt::RoundCap);
        eraserStroker.setJoinStyle(Qt::RoundJoin);
        QPainterPath eraserStroke = eraserStroker.createStroke(m_currentEraserPath);

        for (int i = m_paths.size() - 1; i >= 0; --i)
        {
            QPainterPathStroker pathStroker;
            pathStroker.setWidth(m_paths[i].lineWidth);
            pathStroker.setCapStyle(Qt::RoundCap);
            pathStroker.setJoinStyle(Qt::RoundJoin);
            QPainterPath pathStroke = pathStroker.createStroke(m_paths[i].path);

            if (!eraserStroke.boundingRect().intersects(pathStroke.boundingRect()))
                continue;

            if (eraserStroke.intersects(pathStroke))
            {
                m_undoPathlist.append(m_paths.takeAt(i));
                //m_paths.removeAt(i);
            }
        }

        update();
        return;
    }

    if (m_drawing && !m_paths.isEmpty())
    {
        m_paths.last().path.lineTo(p);
        update();
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_erasing)
    {
        m_currentEraserPath = QPainterPath(); 
        update();
    }
}