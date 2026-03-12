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
};




#endif // CURSORITEM_H
