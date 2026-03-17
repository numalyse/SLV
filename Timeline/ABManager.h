#ifndef ABMANAGER_H
#define ABMANAGER_H


#include "Timeline/TimelineMath.h"

#include "Timeline/Items/ABMarkerItem.h"

#include <QGraphicsScene>
#include <QObject>

struct ABLoopData{
    int64_t aTime;
    double aXPos;
    int64_t bTime;
    double bXPos;
};

class ABManager : public QObject
{
Q_OBJECT

public:
    explicit ABManager(QGraphicsScene* scene, TimelineMath* mathManager, QObject* parent = nullptr);

    std::optional<ABLoopData> getABLoopData();
    std::optional<int64_t> getLoopRestartTime(int64_t currentTime);
    int getMarkerCount() { return static_cast<int>(m_abMarkersItems.size());}

    void cycleMarkers(int64_t time);
    void updateMarkersPosition();

signals:
    void enableSliderRequested();
    void disableSliderRequested();

private:
    QVector<ABMarkerItem*> m_abMarkersItems;

    QGraphicsScene* p_scene = nullptr;
    TimelineMath* p_mathManager = nullptr;
};




#endif