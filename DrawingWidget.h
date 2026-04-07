#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QSize>

class DrawingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void showDrawingMode(bool isEnabled);
    void setColor(const QColor& color);
    void setLineWidth(int width);

public slots:
    void onMediaRectChanged(const QRect &rect);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QRect m_mediaRect;
    bool m_isEnabled = false;
    QColor m_color = QColor(255, 255, 255, 120);
    int m_lineWidth = 2;
};

#endif // DRAWINGWIDGET_H