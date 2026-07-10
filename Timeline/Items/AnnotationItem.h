#ifndef ANNOTATIONITEM_H
#define ANNOTATIONITEM_H

#include "Annotation.h"
#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>

class AnnotationHandleItem;

class AnnotationItem : public QGraphicsItem
{

public:
    explicit AnnotationItem(const Annotation& annot, double width, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Annotation annotation() const { return m_annot; };
    int annotationId() const { return m_annot.id; };
    
    void setWidth(double newWidth);
    void updateAnnotation(const Annotation& annot) {m_annot = annot;};

private:
    Annotation m_annot{};

    double m_width{};
    double m_height{};

    static constexpr double s_height = 17.0;
    static constexpr double s_topMargin = 30.0;
    static constexpr double s_textMargin = 4.0;
};




#endif