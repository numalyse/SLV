#ifndef RULERITEM_H
#define RULERITEM_H

#include <QGraphicsItem>

class RulerItem : public QGraphicsItem
{

public:

    RulerItem(int width, int height);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setSize(int width, int height);

private:
    int m_width;
    int m_height;

};



#endif // RULERITEM_H
