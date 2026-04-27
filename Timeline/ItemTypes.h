#ifndef ITEMTYPES_H
#define ITEMTYPES_H

#include <QGraphicsItem>

namespace SLV {
    enum  ItemTypes
    {
        TypeRulerItem = QGraphicsItem::UserType + 1,
        TypeCursorItem = QGraphicsItem::UserType + 2,
        TypeShotItem = QGraphicsItem::UserType + 3,
        TypeABMarkerItem = QGraphicsItem::UserType + 4,
        TypeAudioVisualizerItem = QGraphicsItem::UserType + 5
    };
}

#endif
