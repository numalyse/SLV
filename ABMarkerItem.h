#ifndef ABMARKERITEM_H
#define ABMARKERITEM_H

#include "ItemTypes.h"

#include <QGraphicsItem>

class ABMarkerItem : public QGraphicsItem
{
public:
    explicit ABMarkerItem(int height, int64_t vlcTime, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override { return SLV::TypeABMarkerItem; }
    int64_t time(){return m_vlcTime;};

private:
    int64_t m_vlcTime;
    int m_height;

    static constexpr QPointF s_points[5] {
        QPointF(1.5*-3.0, 0.0),
        QPointF(1.5*-3.0, 1.5*5.0),
        QPointF(0.0, 1.5*7.5),
        QPointF(1.5*3.0, 1.5*5.0),
        QPointF(1.5*3.0, 0.0),
    }; 

};




#endif // CURSORITEM_H
