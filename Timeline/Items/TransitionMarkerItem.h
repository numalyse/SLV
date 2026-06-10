#ifndef TRANSITIONMARKERITEM_H
#define TRANSITIONMARKERITEM_H

#include "Timeline/ItemTypes.h"
#include "Timeline/Items/RangeMarkerItem.h"

#include <QGraphicsItem>

class TransitionMarkerItem : public RangeMarkerItem
{
public:
    explicit TransitionMarkerItem(int height, int64_t vlcTime, QGraphicsItem* parent = nullptr);
    int type() const override { return SLV::TypeTransitionMarkerItem; }
protected:
    virtual const QPointF* getPolygonPoints() const override { return s_points; }
    virtual int getPolygonPointCount() const override { return 5; }

private:
    int64_t m_vlcTime;
    int m_height;
    QColor m_color = {0, 255, 0};

    static constexpr QPointF s_points[5] {
        QPointF(1.5*-3.0, 0.0),
        QPointF(1.5*-3.0, 1.5*5.0),
        QPointF(0.0, 1.5*7.5),
        QPointF(1.5*3.0, 1.5*5.0),
        QPointF(1.5*3.0, 0.0),
    }; 

};





#endif // TRANSITIONMARKERITEM_H
