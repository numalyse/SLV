#ifndef RANGEMARKERITEM_H
#define RANGEMARKERITEM_H

#include <QGraphicsItem>

class RangeMarkerItem : public QGraphicsItem
{
public:
    explicit RangeMarkerItem(int height, int64_t vlcTime, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int64_t time(){return m_vlcTime;};
    void setTime(const int64_t time){ m_vlcTime = time; }

    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    virtual const QPointF* getPolygonPoints() const = 0;
    virtual int getPolygonPointCount() const = 0;
    
    int64_t m_vlcTime;
    int m_height;
    QColor m_color = {255,140,56};
};




#endif // RANGEMARKERITEM_H
