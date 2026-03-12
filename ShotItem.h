#ifndef SHOTITEM_H
#define SHOTITEM_H

#include "Shot.h"

#include <QGraphicsItem>

class ShotItem : public QGraphicsItem
{
public:
    explicit ShotItem(Shot shot, double width, double height, double topMargin = 40, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setWidth(double width);
    double width(){return m_width;}
    Shot& shot(){return m_shot;};

private:
    Shot m_shot;
    double m_width{};
    double m_height{};
    double m_topMargin{};
};




#endif // SHOTITEM_H
