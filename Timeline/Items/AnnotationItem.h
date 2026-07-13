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
    int type() const override { return SLV::TypeAnnotationItem; }

    Annotation& annotation() { return m_annot; };
    int annotationId() const { return m_annot.id; };
    
    void setWidth(double newWidth);
    double width() { return m_width; };
    void updateAnnotation(const Annotation& annot) {m_annot = annot; updateTextItem();};

    static constexpr double height() { return s_height; }

private:
    void updateTextItem();

    Annotation m_annot{};
    double m_width{};

    QGraphicsTextItem* m_annotTxtItem = nullptr;
    AnnotationHandleItem* m_leftHandle = nullptr;
    AnnotationHandleItem* m_rightHandle = nullptr;

    static constexpr double s_height = 17.0;
    static constexpr double s_topMargin = 30.0;
    static constexpr double s_textMargin = 4.0;
};




#endif