#ifndef SHOTITEM_H
#define SHOTITEM_H

#include "Shot.h"

#include <QGraphicsItem>

class ShotItem : public QGraphicsItem
{
public:
    ShotItem(Shot* shot, int height);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    Shot* shot(){return m_shotData;};

private:
    Shot* m_shotData;
    int m_width;
    int m_height;
    int m_startPos;
};




#endif // SHOTITEM_H
