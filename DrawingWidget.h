#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include "./ToolbarButtons/ToolbarToggleButton.h"
#include "./ToolbarButtons/ToolbarToggleHoverButton.h"
#include "./ToolbarButtons/ToolbarButton.h"
#include <QWidget>
#include <QFrame>
#include <QSize>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QColor>
#include <QButtonGroup>

struct DrawingStroke {
    QPainterPath path;
    QColor color;
    int lineWidth;
};

class DrawingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void showDrawingMode(bool isEnabled);

    void setColor(const QColor &color);
    void updateCurrentLineWidthBtnActive(double lineWidth);
    void setLineWidth(int width);
    void setOpacity(float opacity);

    void enterEvent(QEnterEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void onMediaRectChanged(const QRect &rect);
    void updateToolbarButtonsState();

    
    void updateCurrentOpacityBtnActive(double opacity);

    void binRequested();
    void undoDrawing();
    void redoDrawing();

protected:
    QWidget *m_drawingSurface = nullptr;
    //QFrame * m_drawingSurface = nullptr;
    QImage m_drawingCanvas;

    QWidget *m_drawingToolbar = nullptr;
    QFrame *containerBackground = nullptr;

    void paintEvent(QPaintEvent *event) override;

    void initDrawingSurface();

    QIcon genIconPreviewColor(QColor color, int sizePen = 30);
    void updatePen();
    void initDrawingToolbar();

    QPainterPath m_currentEraserPath;

    void scaleStrokeList(QVector<DrawingStroke> &strokes, double scaleX, double scaleY);
    void scaleCurrentEraserPath(double scaleX, double scaleY);

private:
    QRect m_mediaRect;
    bool m_isEnabled = false;

    QVector<DrawingStroke> m_paths;
    QVector<DrawingStroke> m_redoPathlist;
    QVector<DrawingStroke> m_undoPathlist;
    QVector<DrawingStroke> m_lastClearedPaths;

    QPoint m_lastPoint;
    bool m_hasLastPoint = false;

    bool m_drawing = false;
    bool m_erasing = false;

    ToolbarToggleHoverButton* m_pencilToolBtn = nullptr;
    QVector<ToolbarToggleButton*> m_lineWidthBtns;
    QVector<ToolbarToggleButton*> m_opacityBtns;
    ToolbarToggleHoverButton* m_colorToolBtn = nullptr;
    ToolbarToggleButton* m_eraserToolBtn = nullptr;
    ToolbarButton* m_binToolBtn = nullptr;
    ToolbarButton* m_undoToolBtn = nullptr;
    ToolbarButton* m_redoToolBtn = nullptr;
    ToolbarToggleButton* m_minimizeToolbarBtn = nullptr;
    
    QPen m_pen;

    int m_lineWidth = 4;
    QVector<int> m_lineWidthLevels = {4, 8, 12};

    double m_opacity = 1.0;
    QVector<double> m_opacityLevels = {0.25, 0.5, 1.0};

    QColor m_color = QColor(255, 255, 255, 255);
    QVector<QPair<QString, QColor>> m_palette;

    QPixmap eraseColor();
    QPixmap m_eraseBrush = eraseColor();
    int m_eraserLineWidth = 4;

};

#endif // DRAWINGWIDGET_H