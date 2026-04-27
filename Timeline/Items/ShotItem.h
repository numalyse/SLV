#ifndef SHOTITEM_H
#define SHOTITEM_H

#include "Shot.h"
#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>
#include <QPixmap>

class ShotItem : public QGraphicsItem
{
public:
    explicit ShotItem(Shot shot, double width, double height, double topMargin = 30, QGraphicsItem* parent = nullptr);

    void setThumbnail(const QPixmap &pixmap);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setWidth(double width);
    double width(){return m_width;}
    Shot& shot(){return m_shot;};

   int type() const override { return SLV::TypeShotItem; }

private:
    Shot m_shot;
    QPixmap m_pixmap;
    double m_width{};
    double m_height{};
    double m_topMargin{};

    constexpr static int s_minSizeForImage{100};
};




#endif // SHOTITEM_H
