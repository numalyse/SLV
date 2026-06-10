#ifndef TRANSITIONMANAGER_H
#define TRANSITIONMANAGER_H

#include "Timeline/BaseRangeManager.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/Items/TransitionMarkerItem.h"
#include "Timeline/TimelineView.h"

#include <QGraphicsScene>
#include <QObject>
#include <QVector>

class TransitionManager : public BaseRangeManager<TransitionMarkerItem>
{
Q_OBJECT

public:
    explicit TransitionManager(QGraphicsScene* scene, TimelineMath* mathManager, QObject* parent = nullptr);

private:

    QVector<TransitionMarkerItem*> m_markers;
    //QVector<TransitionItem*> m_transitions;

    TimelineView* p_view = nullptr;
    TimelineMath* p_mathManager = nullptr;

};


#endif
