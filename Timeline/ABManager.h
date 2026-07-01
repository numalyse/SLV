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
    static constexpr uint64_t c_minInterval = 1000;

    explicit ABManager(QGraphicsScene* scene, TimelineMath* mathManager, QObject* parent = nullptr);

    void cycleMarkers(int64_t time, int markerHeight) override;
    void changeMarkerTime(ABMarkerItem *marker, const int64_t time) override;

    std::optional<int64_t> getLoopRestartTime(int64_t currentTime);
    std::optional<int64_t> getDisplayHoldTime(int64_t currentTime) const;
    std::optional<int64_t> clampToLoopRange(int64_t time);
    void extractLoop();

private:
    enum class LoopState {
        Idle,   // jamais entré dans al loop
        InLoop  // lecture bouclée entre A et B
    };

    LoopState m_loopState = LoopState::Idle;

signals:
    void loopExtracted(const QString& outputPath);
    void loopExtractionFailed();

};




#endif
