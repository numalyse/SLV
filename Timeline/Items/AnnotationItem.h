#ifndef ANNOTATIONITEM_H
#define ANNOTATIONITEM_H

#include "Annotation.h"

#include <QGraphicsItem>

class AnnotationItem : public QGraphicsItem
{

public:
    explicit AnnotationItem(const Annotation& annot, double width, double height = 17, double topMargin = 30, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Annotation annotation() const { return m_annot; };
    int annotationId() const { return m_annot.id; };
    double height() const { return m_height; };
    
    void setWidth(double newWidth);
    void updateAnnotation(const Annotation& annot) {m_annot = annot;};

private:
    Annotation m_annot{};

    double m_width{};
    double m_height{};
    double m_topMargin{};
};




#endif