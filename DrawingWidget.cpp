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
        m_drawingSurface->setAttribute(Qt::WA_TranslucentBackground);
        m_drawingSurface->setAttribute(Qt::WA_NoSystemBackground);
        m_drawingSurface->setAttribute(Qt::WA_OpaquePaintEvent, false);
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
    containerBackground->setContentsMargins(0,0,0,0);
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
    m_colorToolBtn->setOnRight(true);
    m_colorToolBtn->setIcon(genIconPreviewColor(m_color));
    drawingToolbarLayout->addWidget(m_colorToolBtn);

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
        "pencil_white",
        PrefManager::instance().getText("tooltip_pencil_tool") + " " + PrefManager::instance().getText("(deactivated)")
    );
    m_pencilToolBtn->setOnRight(true);
    m_pencilToolBtn->setToggledIconFrame(true);
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

        lineWidthBtn->setToggledIconFrame(true);
        if(lineWidth == m_lineWidth)
            lineWidthBtn->setChecked(true);

        lineWidthBtn->setIcon(genIconPreviewColor(Qt::white, lineWidth*2));
        connect(lineWidthBtn, &ToolbarToggleButton::clicked, this, [this, lineWidth]() {
            setLineWidth(lineWidth);
            updatePen();
            updateCurrentLineWidthBtnActive(lineWidth);
        });
        connect(lineWidthBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);

        m_lineWidthBtns.append(lineWidthBtn);
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

        opacityBtn->setToggledIconFrame(true);
        if(opacity == m_opacity)
            opacityBtn->setChecked(true);

        QColor previewColor = Qt::white;
        previewColor.setAlphaF(opacity);

        opacityBtn->setIcon(genIconPreviewColor(previewColor));

        connect(opacityBtn, &ToolbarToggleButton::clicked, this, [this, opacity]() {
            setOpacity(opacity);
            setColor(m_color);
            updatePen();
            updateCurrentOpacityBtnActive(opacity);
        });
        connect(opacityBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);

        m_opacityBtns.append(opacityBtn);
        pencilLayout->addWidget(opacityBtn);
    }

    // BOUTON GOMME
    m_eraserToolBtn = new ToolbarToggleButton(
        m_drawingToolbar,
        false,
        "eraser_white",
        PrefManager::instance().getText("tooltip_eraser_tool") + " " + PrefManager::instance().getText("(activated)"),
        "eraser_white",
        PrefManager::instance().getText("tooltip_eraser_tool") + " " + PrefManager::instance().getText("(deactivated)")
    );
    m_eraserToolBtn->setToggledIconFrame(true);
    connect(m_eraserToolBtn, &ToolbarToggleButton::clicked, this, &DrawingWidget::updateToolbarButtonsState);
    drawingToolbarLayout->addWidget(m_eraserToolBtn);    

    // BOUTON SUPPRIMER TOUT
    m_binToolBtn = new ToolbarButton(
        m_drawingToolbar,
        "delete_white",
        PrefManager::instance().getText("tooltip_bin_tool")
    );
    m_binToolBtn->setEnabled(false);
    connect(m_binToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::binRequested);
    drawingToolbarLayout->addWidget(m_binToolBtn);

    m_drawingToolbar->hide();

    // BOUTON ANNULER
    m_undoToolBtn = new ToolbarButton(
        m_drawingToolbar,
        "undo_white",
        PrefManager::instance().getText("tooltip_undo_tool")
    );
    m_undoToolBtn->setEnabled(false);
    connect(m_undoToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::undoDrawing);
    drawingToolbarLayout->addWidget(m_undoToolBtn);

    // BOUTON RETABLIR
    m_redoToolBtn = new ToolbarButton(
        m_drawingToolbar,
        "redo_white",
        PrefManager::instance().getText("tooltip_redo_tool")
    );
    m_redoToolBtn->setEnabled(false);
    connect(m_redoToolBtn, &ToolbarButton::clicked, this, &DrawingWidget::redoDrawing);
    drawingToolbarLayout->addWidget(m_redoToolBtn);

    // BOUTON MINIMISER TOOLBAR
    m_minimizeToolbarBtn = new ToolbarToggleButton(
        m_drawingToolbar,
        false,
        "plus_white",
        PrefManager::instance().getText("tooltip_expand_toolbar"),
        "minus_white",
        PrefManager::instance().getText("tooltip_minimize_toolbar")
    );
    connect(m_minimizeToolbarBtn, &ToolbarToggleButton::clicked, this, [this]() {
        const bool minimized = m_minimizeToolbarBtn->isChecked();
        m_minimizeToolbarBtn->setChecked(minimized);
        m_minimizeToolbarBtn->setButtonState(minimized);

        m_colorToolBtn->setVisible(!minimized);
        m_eraserToolBtn->setVisible(!minimized);
        m_binToolBtn->setVisible(!minimized);
        m_undoToolBtn->setVisible(!minimized);
        m_redoToolBtn->setVisible(!minimized);
        emit onMediaRectChanged(m_mediaRect);
    });
    drawingToolbarLayout->addWidget(m_minimizeToolbarBtn);

}

// Gestion du changement d'état entre crayon et gomme
void DrawingWidget::updateToolbarButtonsState(){
    ToolbarToggleButton* senderBtn = qobject_cast<ToolbarToggleButton*>(sender());
    //qDebug() << "updateToolbarButtonsState appelé par : " << senderBtn;
    if (senderBtn == m_pencilToolBtn) {
        if (m_pencilToolBtn->isChecked()) {
            m_eraserToolBtn->setChecked(false);
            //m_eraserToolBtn->setButtonState(false);
            m_drawing = true;
            m_erasing = false;
        } else {
            m_drawing = false;
        }
        m_pencilToolBtn->setButtonState(m_pencilToolBtn->isChecked());
        
    } else if (senderBtn == m_eraserToolBtn) {
        if (m_eraserToolBtn->isChecked()) {
            m_pencilToolBtn->setChecked(false);
            //m_pencilToolBtn->setButtonState(false);
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
        }
    }
}

// Génération d'une texture pour la gomme
QPixmap DrawingWidget::eraseColor(){
    QPixmap brush(8, 8);
    brush.fill(Qt::white);
    
    QPainter p(&brush);
    p.fillRect(0, 0, 4, 4, Qt::gray);
    p.fillRect(4, 4, 4, 4, Qt::gray);
    p.end();
    return brush;
}

// Choix couleur du trait
void DrawingWidget::setColor(const QColor &color)
{
    QColor colorWithOpacity = color;
    colorWithOpacity.setAlphaF(m_opacity);
    m_color = colorWithOpacity;

    update();
}

void DrawingWidget::updateCurrentLineWidthBtnActive(double lineWidth){
    for (ToolbarToggleButton* lineWidthBtn : m_lineWidthBtns) {
        if (lineWidthBtn->toolTip().contains(QString::number(lineWidth))) {
            lineWidthBtn->setChecked(true);
        } else {
            lineWidthBtn->setChecked(false);
        }
    }
}

// Choix épaisseur du trait
void DrawingWidget::setLineWidth(int width)
{
    m_lineWidth = width;
    update();
}

void DrawingWidget::updateCurrentOpacityBtnActive(double opacity){
    for (ToolbarToggleButton* opacityBtn : m_opacityBtns) {
        if (opacityBtn->toolTip().contains(QString::number(opacity))) {
            opacityBtn->setChecked(true);
        } else {
            opacityBtn->setChecked(false);
        }
    }
}

void DrawingWidget::setOpacity(float opacity)
{
    m_opacity = opacity;
    update();
}

// Suppression de tous les traits
void DrawingWidget::binRequested(){
    if (m_paths.isEmpty())
        return;

    m_historyPathlist.append(m_paths);
    m_historyIndex++;
    m_paths.clear();
    update();
}

// Gestion de l'historique des dessins

void DrawingWidget::updatePathsFromHistory(){
    if(m_historyIndex >= 0 && m_historyIndex < m_historyPathlist.size()){
        m_paths = m_historyPathlist[m_historyIndex];
    } else {
        m_paths.clear();
    }
}

void DrawingWidget::undoDrawing()
{
    if(m_historyIndex > 0){
        m_historyIndex--;
        updatePathsFromHistory();
        update();

        m_redoToolBtn->setEnabled(true);
        if(m_historyIndex == 0){
            m_undoToolBtn->setEnabled(false);
        }
    }
}

void DrawingWidget::redoDrawing()
{
    if(m_historyIndex + 1 < m_historyPathlist.size()){
        m_historyIndex++;
        updatePathsFromHistory();
        update();

        if(m_historyIndex + 1 >= m_historyPathlist.size()){
            m_redoToolBtn->setEnabled(false);
        }
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
        //scaleStrokeList(m_undoPathlist, scaleX, scaleY);
        //scaleStrokeList(m_redoPathlist, scaleX, scaleY);
        //scaleStrokeList(m_binPathlist, scaleX, scaleY);
        scaleCurrentEraserPath(scaleX, scaleY);
    }

    m_mediaRect = rect;
    if(containerBackground)
        //containerBackground->setGeometry(20, m_mediaRect.height()-200-20, 50, 200);
        containerBackground->adjustSize();
        containerBackground->move(20, m_mediaRect.height()-containerBackground->height()-20);
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
    p.fillRect(rect(), QColor(0,0,0,1));

    // Dessin
    for (const DrawingStroke &stroke : m_paths)
    {
        QPen strokePen(stroke.color, stroke.lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(strokePen);
        p.drawPath(stroke.path);
    }
    

    // Gomme
    if (m_erasing)
    {
        QPen eraser;
        QBrush eraserBrush(m_eraseBrush);
        eraserBrush.setStyle(Qt::TexturePattern);
        eraser.setBrush(eraserBrush);
        eraser.setWidth(m_eraserLineWidth);
        eraser.setStyle(Qt::SolidLine);
        eraser.setCapStyle(Qt::RoundCap);
        eraser.setJoinStyle(Qt::RoundJoin);

        p.setPen(eraser);
        p.drawPath(m_currentEraserPath);
    }
}

void DrawingWidget::enterEvent(QEnterEvent *event)
{
    setFocus(Qt::MouseFocusReason);
    QWidget::enterEvent(event);
    //qDebug() << "ici";
}

void DrawingWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    //qDebug() << "pressed";

    if (event->button() != Qt::LeftButton || !m_isEnabled)
        return;

    QPoint p = event->pos() - m_mediaRect.topLeft();

    if (m_erasing)
    {
        m_currentEraserPath = QPainterPath();
        m_currentEraserPath.moveTo(p);

        update();
        return;
    }

    if (m_drawing)
    {
        DrawingStroke newStroke;
        newStroke.path.moveTo(p);
        newStroke.color = m_color;
        newStroke.lineWidth = m_lineWidth;

        // Ajoute le nouveau trait à m_paths
        m_paths.append(newStroke);

        while(m_historyPathlist.size() > m_historyIndex + 1){
            m_historyPathlist.removeLast();
        }

        m_historyPathlist.append(m_paths);
        m_historyIndex++;

        m_binToolBtn->setEnabled(true);
        m_undoToolBtn->setEnabled(true);
        m_redoToolBtn->setEnabled(false);
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
                m_paths.removeAt(i);

                while(m_historyPathlist.size() > m_historyIndex + 1){
                    m_historyPathlist.removeLast();
                }

                m_historyPathlist.append(m_paths);
                m_historyIndex++;

                m_binToolBtn->setEnabled(!m_paths.isEmpty());
                m_undoToolBtn->setEnabled(!m_paths.isEmpty());
                m_redoToolBtn->setEnabled(false);

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