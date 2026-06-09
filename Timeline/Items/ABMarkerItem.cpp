#include "Timeline/Items/ABMarkerItem.h"

#include <QPainter>
#include <QCursor>

ABMarkerItem::ABMarkerItem(int height, int64_t vlcTime, QGraphicsItem *parent) 
: RangeMarkerItem(height, vlcTime, parent)
{
    setZValue(10);
    setAcceptHoverEvents(true);
}

