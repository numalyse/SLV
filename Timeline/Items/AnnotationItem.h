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
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    int type() const override { return SLV::TypeAnnotationItem; }

    void setPixelsPerMs(double pixelsPerMs);
    void setZeroDurationDisplayWidthMs(double widthMs);
    void setDraggingHandle(AnnotationHandleItem* handle);
    Annotation& annotation() { return m_annot; };
    int annotationId() const { return m_annot.id; };

    void setWidth(double newWidthMs);
    double width() { return m_widthMs; };

    /// @brief returns handle closest to the item center, sceneX in pixels
    AnnotationHandleItem* closestHandle(double sceneX) { 
        const double handleWidthMs = m_widthMs > 0.0
            ? m_widthMs
            : effectiveDisplayWidthMs(m_pixelsPerMs);
        return (sceneX < mapToScene(QPointF(handleWidthMs / 2.0, 0)).x()) ? m_leftHandle : m_rightHandle; 
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
    double effectiveDisplayWidthMs(double pixelsPerMs) const;
    void updateHandlePositions();
    void updateHandleVisibility(const QPointF& mousePos);

    Annotation m_annot{};
    double m_widthMs{};
    double m_zeroDurationDisplayWidthMs{};
    double m_pixelsPerMs{1.0};

    AnnotationHandleItem* m_leftHandle = nullptr;
    AnnotationHandleItem* m_rightHandle = nullptr;

    QString m_elidedText;
    int m_elidedForWidth = -1; 

    static constexpr double s_height = 17.0;
    static constexpr double s_topMargin = 30.0;
    static constexpr double s_textMargin = 4.0;
    static constexpr double s_minVisibleWidthPx = 5.0;
    static constexpr double s_handleHoverMarginPx = 8.0;
};




#endif
