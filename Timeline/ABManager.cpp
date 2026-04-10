#include "Timeline/ABManager.h"
#include "ABManager.h"
#include "PrefManager.h"
#include "SequenceExtractionHelper.h"
#include "Project/ProjectManager.h"

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

    // marge, apres avoir restart la loop, currentTime peut etre legerement inférieur à aTime, on ajoute une frame de marge pour ne pas restart en boucle
    int64_t margin = 1000.0 / p_mathManager->fps();

    if (currentTime >= bTime || currentTime < aTime - margin) {
        return aTime; 
    }

    return {};
}

std::optional<int64_t> ABManager::clampToLoopRange(int64_t time)
{
    if(m_abMarkersItems.size() < 2) return {}; 

    int64_t aTime = m_abMarkersItems[0]->time();
    int64_t bTime = m_abMarkersItems[1]->time();

    return std::clamp(time, aTime, bTime);
}

void ABManager::deleteMarkers()
{
    for (auto* marker : m_abMarkersItems)
    {
        p_scene->removeItem(marker);
        delete marker;
        marker = nullptr;
    }
    m_abMarkersItems.clear();
    emit ABLoopOff();
}

/// @brief Ajoute un marqueur si 0 ou 1 marqueur présent. Si 1 marqueur déjà présent, garde l'ordre tel que element de 0 de m_abMarkersItems est le "A".
/// Si 2 marqueurs présents, les supprimes.
void ABManager::cycleMarkers(int64_t time, int markerHeight){

    switch (m_abMarkersItems.size())
    {
    case 2:{
        deleteMarkers();
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

void ABManager::extractLoop()
{
    if(m_abMarkersItems.size() < 2) return;

    auto& prefManager = PrefManager::instance();
    auto& projManager = ProjectManager::instance();
    QFileInfo mediaFileInfo(projManager.mediaPath());

    if( ! mediaFileInfo.exists() ) return;

    // if the project is saved in a folder, use it else use prefmanager export path
    QString dialogDir = (projManager.projet()->path.isEmpty()) ? prefManager.getPref("Paths", "lp_export") : projManager.projet()->path;

    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr, 
        prefManager.getText("export_file_path_title"), 
        dialogDir
    );

    selectedPath += '.' + mediaFileInfo.suffix();

    SequenceExtractionHelper::extractSequence( mediaFileInfo.filePath() ,m_abMarkersItems[0]->time(), m_abMarkersItems[1]->time(), selectedPath);

}
