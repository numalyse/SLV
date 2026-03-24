#ifndef COMPOSITIONWIDGET_H
#define COMPOSITIONWIDGET_H

#include <QWidget>

class CompositionWidget : public QWidget
{
    Q_OBJECT

public:
    enum OverlayMode {
        None,
        RuleOfThirds,
        CenterCross,
        Diagonals,
        S_Curve,
        GoldenRatio
    };

    bool vertical_flip = false;
    bool horizontal_flip = false;

    explicit CompositionWidget(QWidget *parent = nullptr);

    void setOverlayMode(OverlayMode mode);
    OverlayMode overlayMode() const;

    void setVerticalFlip(bool vf);
    void setHorizontalFlip(bool hf);

    void setColor(const QColor& color);
    void setLineWidth(int width);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawRuleOfThirds(QPainter& p);
    void drawCenterCross(QPainter& p);
    void drawDiagonals(QPainter& p);
    void drawS_Curve(QPainter &p);

    void drawGoldenRatio(QPainter &p);

private:
    OverlayMode m_mode = None;
    QColor m_color = QColor(255, 255, 255, 120);
    int m_lineWidth = 1;
};

#endif // COMPOSITIONWIDGET_H