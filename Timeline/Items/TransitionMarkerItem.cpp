#include "TransitionMarkerItem.h"

TransitionMarkerItem::TransitionMarkerItem(int height, int64_t vlcTime, QGraphicsItem *parent)
    : RangeMarkerItem(height, vlcTime, parent)
{
    setZValue(9);
}