#ifndef RULERITEM_H
#define RULERITEM_H

#include "ItemTypes.h"

#include <QGraphicsItem>

class RulerItem : public QGraphicsItem
{

public:

    explicit RulerItem(int width, int height, double minPxBetweenTicks, double pixelsPerMs, int64_t duration, double fps, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setSize(int width, int height, double pixelsPerMs);

    int type() const override { return SLV::TypeRulerItem;}

private:
    int m_width;
    int m_height;
    double m_minPxBetweenTicks;
    double m_pixelsPerMs;
    double m_fps;
    int64_t m_duration;
    std::vector<double> m_zoomSteps;
    
    void computeZoomSteps(double fps, std::vector<double> &zoomSteps);
};



#endif // RULERITEM_H
