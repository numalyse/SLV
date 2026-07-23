#ifndef MEDIALOGOWIDGET_H
#define MEDIALOGOWIDGET_H

#include "AspectRatioPixmapLabel.h"

#include <QWidget>
#include <QSize>

class QVBoxLayout;

class MediaLogoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaLogoWidget(QWidget *parent, const QString& iconPath, int maxWidth);

    // Layout below the logo where we can add text, buttons, etc.
    // Empty by default (logo only).
    QVBoxLayout* contentLayout() const { return m_contentLayout; }

public slots:
    void paintEvent(QPaintEvent *event);
    void onMediaRectChanged(const QRect &rect);

    void setDisplay(bool isShown);
    void setIcon(const QString& iconPath, int maxWidth);
    // Show/hide the widgets added to contentLayout() (logo stays visible).
    void setContentVisible(bool visible);

private:
    QRect m_mediaRect;
    AspectRatioPixmapLabel* m_logo = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
    bool m_isShown = false;

};

#endif // MEDIALOGOWIDGET_H