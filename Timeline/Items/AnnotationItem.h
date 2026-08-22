#ifndef ANNOTATIONITEM_H
#define ANNOTATIONITEM_H

#include "Annotation.h"
#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>

class AnnotationHandleItem;

class AnnotationItem : public QGraphicsItem
{

public:
    explicit AnnotationItem(const Annotation& annot, double widthMs, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    int type() const override { return SLV::TypeAnnotationItem; }

    Annotation& annotation() { return m_annot; };
    int annotationId() const { return m_annot.id; };

    void setWidth(double newWidthMs);
    double width() { return m_widthMs; };

    /// @brief returns handle closest to the item center, sceneX in pixels
    AnnotationHandleItem* closestHandle(double sceneX) { 
        return (sceneX < mapToScene(QPointF(m_widthMs / 2.0, 0)).x()) ? m_leftHandle : m_rightHandle; 
    };
    
    void updateAnnotation(const Annotation& annot) {
        m_annot = annot;
        invalidateTextCache();
        update();
    };

    static constexpr double height() { return s_height; }

private:
    /// @brief forces paint() to elide the text on next draw
    void invalidateTextCache() { m_elidedForWidth = -1; }

    Annotation m_annot{};
    double m_widthMs{};

    AnnotationHandleItem* m_leftHandle = nullptr;
    AnnotationHandleItem* m_rightHandle = nullptr;

    QString m_elidedText;
    int m_elidedForWidth = -1; 

    static constexpr double s_height = 17.0;
    static constexpr double s_topMargin = 30.0;
    static constexpr double s_textMargin = 4.0;
};




#endif
