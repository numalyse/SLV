#ifndef CURSORITEM_H
#define CURSORITEM_H

#include "ItemTypes.h"

#include <QGraphicsItem>

class CursorItem : public QGraphicsItem
{
public:
    explicit CursorItem(int height, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setHeight(int height);
    int type() const override { return SLV::TypeCursorItem; }

private:
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
