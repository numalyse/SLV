#ifndef ANNOTATIONHANDLEITEM_H
#define ANNOTATIONHANDLEITEM_H

#include "Timeline/ItemTypes.h"
#include "Timeline/Items/AnnotationItem.h"

#include <QGraphicsItem>

class AnnotationHandleItem : public QGraphicsItem
{

public:
    explicit AnnotationHandleItem(bool isLeftHandle, QGraphicsItem* parent);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override { 
        if (m_isLeft) return SLV::TypeAnnotationHandleStartItem;
        else return SLV::TypeAnnotationHandleEndItem ; 
    }

    const QPointF* getPolygonPoints() const { return s_points; }
    int getPolygonPointCount() const { return 12; }

private:
    bool m_isLeft = true;

    static constexpr double s_height = AnnotationItem::height();
    static constexpr double s_yScale = s_height / 10.0;

    static constexpr QPointF s_points[12] {
        QPointF(1.5*-1.0, s_yScale*0.0),
        QPointF(1.5*-1.0, s_yScale*3.0),
        QPointF(1.5*-3.0, s_yScale*3.0),
        QPointF(1.5*-3.0, s_yScale*7.0),
        QPointF(1.5*-1.0, s_yScale*7.0),
        QPointF(1.5*-1.0, s_yScale*10.0),
        QPointF(1.5*1.0,  s_yScale*10.0),
        QPointF(1.5*1.0,  s_yScale*7.0),
        QPointF(1.5*3.0,  s_yScale*7.0),
        QPointF(1.5*3.0,  s_yScale*3.0),
        QPointF(1.5*1.0,  s_yScale*3.0),
        QPointF(1.5*1.0,  s_yScale*0.0),
    };

};



#endif