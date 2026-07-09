#ifndef ANNOTATIONITEM_H
#define ANNOTATIONITEM_H

#include "Annotation.h"

#include <QGraphicsItem>

class AnnotationItem : public QGraphicsItem
{
Q_OBJECT

public:
    explicit AnnotationItem(double position, double width, double height, double topMargin = 30, QGraphicsItem* parent = nullptr);
    ~AnnotationItem();

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setAnnotation(const Annotation& annot) {m_annot = annot;};
    void setPosition(double newPos);
    void setWidth(double newWidth);

private:
    Annotation m_annot{};

    double m_position{};
    double m_width{};
    double m_height{};
    double m_topMargin{};

};




#endif