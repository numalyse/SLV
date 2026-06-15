#ifndef MEDIALOGOWIDGET_H
#define MEDIALOGOWIDGET_H

#include "AspectRatioPixmapLabel.h"

#include <QWidget>
#include <QSize>


class MediaLogoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaLogoWidget(QWidget *parent, const QString& iconPath);

public slots:
    void paintEvent(QPaintEvent *event);
    void onMediaRectChanged(const QRect &rect);

    void setDisplay(bool isShown);

private:
    QRect m_mediaRect;
    AspectRatioPixmapLabel* m_logo = nullptr;
    bool m_isShown = false;

};

#endif // MEDIALOGOWIDGET_H