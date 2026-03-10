#ifndef RULERITEM_H
#define RULERITEM_H

#include <QGraphicsItem>

class RulerItem : public QGraphicsItem
{

public:

    explicit RulerItem(int width, int height, double minPxBetweenTicks, QGraphicsItem* parent= nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setSize(int width, int height);

private:
    int m_width;
    int m_height;
    double m_minPxBetweenTicks;
    
    void computeZoomSteps(double newFps, double &cachedFps, std::vector<int64_t> &zoomSteps);
};



#endif // RULERITEM_H
