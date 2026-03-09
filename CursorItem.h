#ifndef CURSORITEM_H
#define CURSORITEM_H

#include <QGraphicsItem>

class CursorItem : public QGraphicsItem
{
public:
    CursorItem(int height);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setHeight(int height);
    int m_height;
private:

};




#endif // CURSORITEM_H
