#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include "./ToolbarButtons/ToolbarToggleButton.h"
#include <QWidget>
#include <QSize>
#include <QPen>

class DrawingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void showDrawingMode(bool isEnabled);
    void setColor(const QColor& color);
    void setLineWidth(int width);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void onMediaRectChanged(const QRect &rect);

protected:
    QWidget *m_drawingToolbar = nullptr;

    void paintEvent(QPaintEvent *event) override;
    void initDrawingToolbar();

private:
    QRect m_mediaRect;
    bool m_isEnabled = false;

    QVector<QPainterPath> m_paths;
    bool m_drawing = false;
    QPoint m_lastPoint;

    ToolbarToggleButton* m_penModeBtn = nullptr;

    QColor m_color = QColor(255, 255, 255, 120);
    int m_lineWidth = 2;
    QPen m_pen {QColor(255, 255, 255, 120), 2};

    
};

#endif // DRAWINGWIDGET_H