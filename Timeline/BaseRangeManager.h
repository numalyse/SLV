#ifndef BASERANGEMANAGER_H
#define BASERANGEMANAGER_H

#include "Timeline/TimelineMath.h"
#include "Timeline/Items/RangeMarkerItem.h" 

#include <QGraphicsScene>
#include <QObject>
#include <QVector> 
#include <optional>
#include <cstdint>  

struct RangeData {
    int64_t aTime;
    double aXPos;
    int64_t bTime;
    double bXPos;
};

class BaseRangeManagerObject : public QObject
{
    Q_OBJECT

public:
    explicit BaseRangeManagerObject(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void onPairCompleted(); 
    void onMarkersCleared(); 
};

// since we can't template a QObject, we templatize the BaseRangeManager and have it inherit from a non-templated QObject (BaseRangeManagerObject)
template <typename MARKERTYPE>
class BaseRangeManager : public BaseRangeManagerObject
{
public:
    explicit BaseRangeManager(QGraphicsScene* scene, TimelineMath* mathManager, QObject* parent = nullptr)
        : BaseRangeManagerObject(parent), p_scene{scene}, p_mathManager{mathManager}
    {
    }

    int getMarkerCount() { return static_cast<int>(m_markers.size()); }
    
    std::optional<RangeData> getRangeData()
    {
        if (m_markers.size() < 2) return std::nullopt; 

        return RangeData{m_markers[0]->time(), m_markers[0]->pos().x(), m_markers[1]->time(), m_markers[1]->pos().x()};
    }

    void deleteMarkers()
    {
        for (auto* marker : m_markers)
        {
            p_scene->removeItem(marker);
            delete marker;
        }
        m_markers.clear();
        emit onMarkersCleared();
    }

    void cycleMarkers(int64_t time, int markerHeight)
    {
        switch (m_markers.size())
        {
            case 2: {
                deleteMarkers();
                break;
            }
            case 1: {
                MARKERTYPE* newMarker = new MARKERTYPE(markerHeight, time);

                if (newMarker->time() >= m_markers[0]->time()) {
                    m_markers.append(newMarker); 
                } else if (newMarker->time() < m_markers[0]->time()) {
                    m_markers.insert(0, newMarker);
                }

                newMarker->setX(p_mathManager->timeToPos(time));
                p_scene->addItem(newMarker);
                emit onPairCompleted();
                break;
            }
            case 0: {
                MARKERTYPE* newMarker = new MARKERTYPE(markerHeight, time);

                m_markers.append(newMarker);
                newMarker->setX(p_mathManager->timeToPos(time));
                p_scene->addItem(newMarker);
                break;
            }
        }
    }

    void changeMarkerTime(MARKERTYPE *marker, const int64_t time)
    {
        marker->setTime(time);
        int markerIndex = m_markers.indexOf(marker); 
        
        if (m_markers.size() == 2) {
            switch (markerIndex) {
                case 0:
                    if (marker->time() >= m_markers[1 - markerIndex]->time())
                        marker->setTime(m_markers[1 - markerIndex]->time() - 1000.0 / p_mathManager->fps());
                    break;
                case 1:
                    if (marker->time() <= m_markers[1 - markerIndex]->time())
                        marker->setTime(m_markers[1 - markerIndex]->time() + 1000.0 / p_mathManager->fps());
                    break;
            }
        }

        updateMarkersPosition();
    }

    void updateMarkersPosition()
    {
        double newXPos{};
        for (auto* marker : m_markers) {
            newXPos = marker->time() * p_mathManager->pixelsPerMs();
            marker->setX(newXPos);
        }
    }

protected:
    QVector<MARKERTYPE*> m_markers;

    QGraphicsScene* p_scene = nullptr;
    TimelineMath* p_mathManager = nullptr;
};

#endif // BASERANGEMANAGER_H