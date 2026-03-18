#include "Timeline/ShotManager.h"

#include "ShotManager.h"
#include "TimeFormatter.h"

ShotManager::ShotManager(QGraphicsScene* scene, TimelineView* view, TimelineMath* mathManager, QVector<Shot> &projectShots, QObject *parent) 
: QObject(parent) ,p_scene{scene}, p_view{view}, p_mathManager{mathManager}
{
    int shotHeight {50};

    for ( auto& IShot : projectShots ){

        double xPos =  p_mathManager->timeToPos(IShot.start);
        double width = p_mathManager->timeToPos(IShot.end - IShot.start);
        
        ShotItem* shot = new ShotItem(IShot, width, shotHeight);

        p_scene->addItem(shot);
        shot->setX(xPos);

        m_shotItems.append(shot);

    }
}



/// @brief Parcours les plan pour trouver le plan courant de gauche a droite pour trouver le plan à la ms actuelle
void ShotManager::updateCurrentShot(int64_t time){
    int shotItemCount = static_cast<int>(m_shotItems.size());

    Q_ASSERT(shotItemCount >= 1);

    int currentShotId = 0;
    int64_t distanceToClosest = time - m_shotItems[0]->shot().start;

    for (int IShot = 1; IShot < shotItemCount; ++IShot){
        
        int64_t currentShotStart = m_shotItems[IShot]->shot().start;

        if( time < currentShotStart ) break; // si le vlc time < start on est avant le plan donc on quitte la boucle

        int64_t distance = time - currentShotStart;
        if(distance < distanceToClosest){
            distanceToClosest = distance;
            currentShotId = IShot;
        }
    }
    ShotItem* closestShotItem =  m_shotItems[currentShotId];
    if(m_currentShotItem != closestShotItem){
        m_currentShotItem =  closestShotItem;
        emit updateShotDetailRequested( shotItemCount, currentShotId, &m_shotItems[currentShotId]->shot());
    }
} 




/// @brief Raccourcis le plan courant et créer une nouveau plan avec comme début la position du curseur
void ShotManager::splitShotAt( int64_t cutTime ) {
    qDebug() << "Ms where to cut " << cutTime;
    
    if (!m_currentShotItem) {
        qDebug() << "Aucun plan courant";
        return;
    }

    int index = m_shotItems.indexOf(m_currentShotItem);
    if (index == -1) {
        qDebug() << "Impossible de retrouver l'index du plan courant dans la liste des shotItems";
        return;
    }

    if (cutTime <=  m_shotItems[index]->shot().start || cutTime >=  m_shotItems[index]->shot().end) {
        qDebug() << "Cut time :" <<  TimeFormatter::msToHHMMSSFF(cutTime, 1);
        qDebug() << "Current shot start :" << TimeFormatter::msToHHMMSSFF(m_shotItems[index]->shot().start, 1);
        qDebug() << "current shot end : " << TimeFormatter::msToHHMMSSFF(m_shotItems[index]->shot().end, 1);
        return;
    }

    auto oldEnd =  m_shotItems[index]->shot().end;
    m_shotItems[index]->shot().end = cutTime - 1; 

    double newWidth1 = p_mathManager->timeToPos( m_shotItems[index]->shot().end - m_shotItems[index]->shot().start );
    m_currentShotItem->setWidth(newWidth1);

    Shot newShotData =  Shot{ "Titre", cutTime, oldEnd};

    double pos2 = p_mathManager->timeToPos(newShotData.start);
    double width2 = p_mathManager->timeToPos(newShotData.end) - pos2;
    int startShotHeight = 50;

    ShotItem* newShotItem = new ShotItem(newShotData, width2, startShotHeight);
    newShotItem->setPos(pos2, m_currentShotItem->y());

    // on insère le plan juste apres le plan cut
    m_shotItems.insert(index + 1, newShotItem);
    p_scene->addItem(newShotItem);

    updateCurrentShot(cutTime);
}


/// @brief met à jour la position / taille des plans, pendant la mise à jour des positions, désactive la mise à jour de l'affichage
void ShotManager::updateShotItemsPosition(){
    if(!p_scene || !p_view) return;

    p_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    p_view->setUpdatesEnabled(false);

    double newXPos{};
    double newWidth{};

    for(auto* shotItem : m_shotItems){

        newXPos = shotItem->shot().start * p_mathManager->pixelsPerMs();
        shotItem->setX(newXPos);

        newWidth = (shotItem->shot().end - shotItem->shot().start) * p_mathManager->pixelsPerMs();
        shotItem->setWidth(newWidth);
    }

    p_view->setUpdatesEnabled(true);
    p_scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    p_scene->update();
}


std::optional<double> ShotManager::getStartXOf(int idShot){
    if(idShot < 0 || idShot >= m_shotItems.size() ) return {};

    return p_mathManager->timeToPos(m_shotItems[idShot]->shot().start); 
}