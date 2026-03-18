#include "Timeline/ABManager.h"
#include "ABManager.h"

ABManager::ABManager(QGraphicsScene *scene, TimelineMath *mathManager, QObject *parent)
: QObject(parent), p_scene{scene}, p_mathManager{mathManager}
{
}

std::optional<ABLoopData> ABManager::getABLoopData()
{
    if (m_abMarkersItems.size() < 2) return {};

    return ABLoopData{m_abMarkersItems[0]->time(), m_abMarkersItems[0]->pos().x(), m_abMarkersItems[1]->time(), m_abMarkersItems[1]->pos().x()};
}

std::optional<int64_t> ABManager::getLoopRestartTime(int64_t currentTime)
{
    if(m_abMarkersItems.size() < 2) return {};

    int64_t aTime = m_abMarkersItems[0]->time();
    int64_t bTime = m_abMarkersItems[1]->time();

    if (currentTime >= bTime || currentTime < aTime) {
        return aTime; 
    }

    return {};

}

/// @brief Ajoute un marqueur si 0 ou 1 marqueur présent. Si 1 marqueur déjà présent, garde l'ordre tel que element de 0 de m_abMarkersItems est le "A".
/// Si 2 marqueurs présents, les supprimes.
void ABManager::cycleMarkers(int64_t time, int markerHeight){

    switch (m_abMarkersItems.size())
    {
    case 2:{
        for (auto* marker : m_abMarkersItems)
        {
            p_scene->removeItem(marker);
            delete marker;
            marker = nullptr;
        }
        m_abMarkersItems.clear();
        emit ABLoopOff();
        break;
    }
    case 1 :{
        ABMarkerItem* newMarker = new ABMarkerItem(markerHeight, time);

        if(newMarker->time() >= m_abMarkersItems[0]->time()){
            m_abMarkersItems.append(newMarker); 
        }else if(newMarker->time() < m_abMarkersItems[0]->time()){
            m_abMarkersItems.insert(0, newMarker);
        }

        newMarker->setX(p_mathManager->timeToPos(time));
        p_scene->addItem(newMarker);
        emit ABLoopOn();
        break;
    }
    case 0 :{
        ABMarkerItem* newMarker = new ABMarkerItem(markerHeight, time);

        m_abMarkersItems.append(newMarker);
        newMarker->setX(p_mathManager->timeToPos(time));
        p_scene->addItem(newMarker);
        break;
    }

    }
}



void ABManager::updateMarkersPosition(){
    double newXPos{};
    for( auto* marker : m_abMarkersItems){
        newXPos = marker->time() * p_mathManager->pixelsPerMs();
        marker->setX(newXPos);
    }
}