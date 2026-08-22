#ifndef TIMELINELAYERITEM_H
#define TIMELINELAYERITEM_H

#include <QGraphicsItem>


/// @brief Invisible container item. Its children are positioned in ms at creation
/// the layer s transform then converts the time to pixels
/// This container is required because an item's transform does not affect its own local
/// geometry
class TimelineLayerItem : public QGraphicsItem
{
public:

    explicit TimelineLayerItem(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {
        setFlag(QGraphicsItem::ItemHasNoContents);
    }

    /// @brief prevents from being hit tested 
    QRectF boundingRect() const override { return QRectF(); }

    /// @brief not painted
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override {}
};

#endif // TIMELINELAYERITEM_H
