#ifndef ABMARKERITEM_H
#define ABMARKERITEM_H

#include "Timeline/ItemTypes.h"

#include "Timeline/Items/RangeMarkerItem.h"

#include <QGraphicsItem>

class ABMarkerItem : public RangeMarkerItem
{
public:
    explicit ABMarkerItem(int height, int64_t vlcTime, QGraphicsItem* parent = nullptr);

    int type() const override { return SLV::TypeABMarkerItem; }

protected:
    virtual const QPointF* getPolygonPoints() const override { return s_points; }
    virtual int getPolygonPointCount() const override { return 5; }

private:

    static constexpr QPointF s_points[5] {
        QPointF(1.5*-3.0, 0.0),
        QPointF(1.5*-3.0, 1.5*5.0),
        QPointF(0.0, 1.5*7.5),
        QPointF(1.5*3.0, 1.5*5.0),
        QPointF(1.5*3.0, 0.0),
    }; 

};




#endif // CURSORITEM_H
