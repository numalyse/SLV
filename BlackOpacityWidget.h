#ifndef BLACKOPACITYWIDGET_H
#define BLACKOPACITYWIDGET_H

#include <QWidget>
#include <QSize>

class BlackOpacityWidget : public QWidget
{
    Q_OBJECT

public:

    explicit BlackOpacityWidget(QWidget *parent = nullptr);

    void initSurface();

public slots:
    void setBlackOpacityMode(bool isShown, double opacity);
    void paintEvent(QPaintEvent *event);
    void onMediaRectChanged(const QRect &rect);

protected:
    bool m_isShown = false;
    QWidget *m_surface = nullptr;
    QColor m_blackColor = Qt::black;
    double m_opacityBlackFrame = 1.0;

private:
    QRect m_mediaRect;

};

#endif BLACKOPACITYWIDGET_H