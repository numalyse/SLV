#ifndef ABMANAGER_H
#define ABMANAGER_H


#include "Timeline/TimelineMath.h"
#include "Timeline/BaseRangeManager.h"
#include "Timeline/Items/ABMarkerItem.h"

#include <QGraphicsScene>
#include <QObject>

class ABManager : public BaseRangeManager<ABMarkerItem>
{
Q_OBJECT

public:
    explicit ABManager(QGraphicsScene* scene, TimelineMath* mathManager, QObject* parent = nullptr);

    std::optional<int64_t> getLoopRestartTime(int64_t currentTime);
    std::optional<int64_t> clampToLoopRange(int64_t time);
    void extractLoop();
};




#endif
