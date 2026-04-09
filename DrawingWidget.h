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
    void setColor(const QColor& color);
    void setLineWidth(int width);

    void enterEvent(QEnterEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void onMediaRectChanged(const QRect &rect);
    void updateToolbarButtonsState();

    void binRequested();

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

private:
    QRect m_mediaRect;
    bool m_isEnabled = false;

    QVector<DrawingStroke> m_paths;
    QVector<DrawingStroke> m_redoPathlist;
    QVector<DrawingStroke> m_undoPathlist;

    QPoint m_lastPoint;

    bool m_drawing = false;
    bool m_erasing = false;

    ToolbarToggleButton* m_pencilToolBtn = nullptr;
    ToolbarToggleHoverButton* m_colorToolBtn = nullptr;
    ToolbarToggleButton* m_eraserToolBtn = nullptr;
    ToolbarButton* m_binToolBtn = nullptr;

    QVector<QPair<QString, QColor>> m_palette;
    QColor m_color = QColor(255, 0, 0, 255);
    int m_lineWidth = 2;
    QPen m_pen {QColor(255, 0, 0, 255), 2};


    
};

#endif // DRAWINGWIDGET_H