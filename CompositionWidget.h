#ifndef COMPOSITIONWIDGET_H
#define COMPOSITIONWIDGET_H

#include "OverlayMode.h"
#include <QWidget>
#include <QSize>

class CompositionWidget : public QWidget
{
    Q_OBJECT

public:

    explicit CompositionWidget(QWidget *parent = nullptr);

    void setOverlayMode(OverlayMode mode, bool isVFlipped, bool isHFlipped);
    OverlayMode overlayMode() const;

    void setVerticalFlip(bool vf);
    void setHorizontalFlip(bool hf);

    void setColor(const QColor& color);
    void setLineWidth(int width);

public slots:
    void onMediaSizeChanged(const QSize &size);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawRuleOfThirds(QPainter& p);
    void drawCenterCross(QPainter& p);
    void drawDiagonals(QPainter& p);
    void drawS_Curve(QPainter &p);

    void drawGoldenRatio(QPainter &p);

    QSize m_mediaSize;
    OverlayMode m_mode = OverlayMode::None;
    bool m_isVFlipped = false;
    bool m_isHFlipped = false;
    QColor m_color = QColor(255, 255, 255, 120);
    int m_lineWidth = 1;
};

#endif // COMPOSITIONWIDGET_H