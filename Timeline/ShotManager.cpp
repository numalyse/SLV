#include "Timeline/ShotManager.h"

#include "ShotManager.h"
#include "TimeFormatter.h"
#include "PrefManager.h"

#include <QtAssert>
#include <QFileDialog>


ShotManager::ShotManager(QGraphicsScene* scene, TimelineView* view, TimelineMath* mathManager, ThumbnailWorker* thumbnailWorker, Media* media, QVector<Shot> &projectShots, QObject *parent) 
: QObject(parent) ,p_scene{scene}, p_view{view}, p_mathManager{mathManager}, p_thumbnailWorker{thumbnailWorker}, p_media{media}
{
    connect(p_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &ShotManager::updateThumbnail);
    connect(&m_videoCaptureManager, &VideoCaptureManager::recordSegmentDone, this, &ShotManager::shotsExtractionFinished);
    connect(&m_videoCaptureManager, &VideoCaptureManager::recordSegmentFailed, this, &ShotManager::shotsExtractionFailed);

    setShotItemsData(projectShots);
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

    if(currentShotId == 0){
        emit showMergeWithPreviousShotAction( false );
    }else {
        emit showMergeWithPreviousShotAction( true );
    }
    if(currentShotId == m_shotItems.size() - 1){
        emit showMergeWithNextShotAction( false );
    }else {
        emit showMergeWithNextShotAction( true );
    }

} 


void ShotManager::initShotDetail(){
    int currentShotId = m_shotItems.indexOf(m_currentShotItem);
    emit updateShotDetailRequested(static_cast<int>(m_shotItems.size()), currentShotId, &m_shotItems[currentShotId]->shot());
}

void ShotManager::toggleSelection(ShotItem* shotItemToSelect, AudioShotItem* audioShotItemToSelect, bool exclusive)
{
    int shotId = -1;
    int audioShotId = -1;

    if( shotItemToSelect ){
        shotId = m_shotItems.indexOf(shotItemToSelect);
        if(shotId != -1) audioShotItemToSelect = m_audioShotItems[shotId];
        else {
            qDebug() << " [SHOTMANAGER]  Erreur, impossible de trouver l'index du shot selectionné";
            return;
        }
    }else if ( !shotItemToSelect && audioShotItemToSelect ) {

        audioShotId = m_audioShotItems.indexOf(audioShotItemToSelect);
        if(audioShotId != -1) shotItemToSelect = m_shotItems[audioShotId];
        else {
            qDebug() << " [SHOTMANAGER]  Erreur, impossible de trouver l'index du shot audio selectionné";
            return;
        }
    }

    QPair<ShotItem*, AudioShotItem*> selected {shotItemToSelect, audioShotItemToSelect};
    int selectedId = m_selectedShots.indexOf(selected); 

    if(exclusive) {

        bool wasTheOnlySelected = (m_selectedShots.size() == 1 && m_selectedShots.first() == selected);

        clearSelection();

        if ( ! wasTheOnlySelected ){ // si on reclique sur le meme plan, on le désélectionne
            addSelected(selected);
        }
    
    }else {
        if(selectedId == -1){
            addSelected(selected);
        }else {
            removeSelected(selectedId, selected);
            updateSelectedNumbers();
        }
    }
}

void ShotManager::updateSelectedNumbers(){
    for(int ISelectedShots = 0; ISelectedShots < m_selectedShots.size(); ++ISelectedShots){
        m_selectedShots[ISelectedShots].first->setSelectedNumber(ISelectedShots+1);
        m_selectedShots[ISelectedShots].second->setSelectedNumber(ISelectedShots+1);
    }
}

void ShotManager::addSelected( QPair<ShotItem*, AudioShotItem*>& toBeAdded){
    m_selectedShots.append(toBeAdded);
    toBeAdded.first->setSelected(true);
    toBeAdded.first->setSelectedNumber(m_selectedShots.size());
    toBeAdded.second->setSelected(true);
    toBeAdded.second->setSelectedNumber(m_selectedShots.size());
}

void ShotManager::removeSelected( int elementIdToRemove, QPair<ShotItem*, AudioShotItem*>& toBeRemoved ){
    toBeRemoved.first->setSelected(false);
    toBeRemoved.second->setSelected(false);

    m_selectedShots.removeAt(elementIdToRemove);
}


void ShotManager::clearSelection()
{
    for(auto& selected : m_selectedShots){
        selected.first->setSelected(false);
        selected.second->setSelected(false);
    }
    m_selectedShots.clear();
}

void ShotManager::extractShotsSelected(const QString& outputPath)
{

    m_videoCaptureManager.setMediaPath(p_media->filePath());
    m_videoCaptureManager.initMediaTempDirectory();

    m_videoCaptureManager.startMediaRecording(m_selectedShots.first().first->shot().start);

    for(int ISelectedShot = 1; ISelectedShot < m_selectedShots.size(); ++ISelectedShot){
        m_videoCaptureManager.mediaCutAndConcat(m_selectedShots[ISelectedShot-1].first->shot().end, m_selectedShots[ISelectedShot].first->shot().start);
    }
    m_videoCaptureManager.endMediaRecording(m_selectedShots.last().first->shot().end, outputPath);


}

/// @brief Shortens the current shot to cursor time - 1ms, create a new shot with start time = cursor time 
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

    QString baseTitle = PrefManager::instance().getText("shot_detail_title_name");

    Shot& baseShot = m_shotItems[index]->shot();

    auto oldEnd =  baseShot.end;
    baseShot.end = cutTime - 1; 
    baseShot.tagImageTime = baseShot.middle();
    m_audioShotItems[index]->shot().end = cutTime - 1;

    double newWidth1 = p_mathManager->timeToPos(baseShot.end - baseShot.start );
    m_currentShotItem->setWidth(newWidth1);
    m_audioShotItems[index]->setWidth(newWidth1);

    Shot newShotData =  Shot{ baseTitle, cutTime, oldEnd};
    newShotData.tagImageTime = newShotData.middle();

    double pos2 = p_mathManager->timeToPos(newShotData.start);
    double width2 = p_mathManager->timeToPos(newShotData.end) - pos2;
    int startShotHeight = 40;

    ShotItem* newShotItem = new ShotItem(newShotData, width2, startShotHeight);
    newShotItem->setPos(pos2, m_currentShotItem->y());

    AudioShot newAudioShotData = AudioShot{};
    newAudioShotData.title = baseTitle;
    newAudioShotData.start = cutTime;
    newAudioShotData.end = oldEnd;

    AudioShotItem* newAudioShotItem = new AudioShotItem(newAudioShotData, width2);
    newAudioShotItem->setPos(pos2, m_currentShotItem->y());

    // insert the shot right after the shot cut
    m_shotItems.insert(index + 1, newShotItem);
    m_audioShotItems.insert(index + 1, newAudioShotItem);

    requestShotThumbnail(index + 1, newShotData);
    // need to update the previous too, if display thumbnails via tagFrames, thumbnail would be outdated 
    // if the new shot duration now < offset, thumbnail would be outdated 
    requestShotThumbnail(index, baseShot);

    p_scene->addItem(newShotItem);
    p_scene->addItem(newAudioShotItem);

    emit shotCountUpdated(shotCount());

    updateCurrentShot(cutTime);
}

void ShotManager::mergeCurrentInto(int ShotItemId){
    Q_ASSERT( ShotItemId >= 0 && ShotItemId < m_shotItems.size() );

    ShotItem* item =  m_shotItems[ShotItemId];

    item->shot().start = (m_currentShotItem->shot().start > item->shot().start) ? item->shot().start: m_currentShotItem->shot().start ;
    item->shot().end = (m_currentShotItem->shot().end > item->shot().end) ? m_currentShotItem->shot().end : item->shot().end ;

    item->shot().note += "\n" + m_currentShotItem->shot().note; // add the note of the current shot to the merged shot

    item->shot().tagImageTime = item->shot().middle(); // updates the tagimage to the middle of the merged shot

    AudioShotItem* audioItem = m_audioShotItems[ShotItemId];
    audioItem->shot().start = item->shot().start;
    audioItem->shot().end = item->shot().end;
    audioItem->shot().note = item->shot().note;

    p_scene->removeItem(m_currentShotItem);
    ShotItem* currentAudioShotItem = m_audioShotItems[m_shotItems.indexOf(m_currentShotItem)];
    p_scene->removeItem(currentAudioShotItem);
    m_shotItems.removeOne(m_currentShotItem);
    m_audioShotItems.removeOne(currentAudioShotItem);
    m_currentShotItem = nullptr;

    requestShotThumbnail(m_shotItems.indexOf(item), item->shot());

    updateShotItemsPosition();
}

void ShotManager::mergeCurrentWithAdjacentShot(int indexOffset, int64_t cursorTime)
{
    Q_ASSERT( m_currentShotItem );

    int targetIndex = m_shotItems.indexOf(m_currentShotItem) + indexOffset;

    mergeCurrentInto(targetIndex);
    clearSelection();
    updateCurrentShot(cursorTime);
    emit shotCountUpdated(shotCount());
}

void ShotManager::mergeCurrentWithPrevShot(int64_t cursorTime)
{
    mergeCurrentWithAdjacentShot(-1, cursorTime);
}

void ShotManager::mergeCurrentWithNextShot(int64_t cursorTime)
{
    mergeCurrentWithAdjacentShot(1, cursorTime);
}



/// @brief Updates items position and width, disable scene view updates while updating 
void ShotManager::updateShotItemsPosition(){
    if(!p_scene || !p_view) return;

    double newXPos{};
    double newWidth{};

    p_view->setUpdatesEnabled(false);

    for (int IShot = 0; IShot < m_shotItems.size(); ++IShot)
    {
        ShotItem* shotItem = m_shotItems[IShot];
        AudioShotItem* audioShotItem = m_audioShotItems[IShot];

        newXPos = shotItem->shot().start * p_mathManager->pixelsPerMs();
        shotItem->setX(newXPos);
        audioShotItem->setX(newXPos);

        newWidth = (shotItem->shot().end - shotItem->shot().start) * p_mathManager->pixelsPerMs();
        shotItem->setWidth(newWidth);
        audioShotItem->setWidth(newWidth);
    }
    
    p_view->setUpdatesEnabled(true);
    p_scene->update();
}

std::optional<double> ShotManager::getStartXOf(int idShot){
    if(idShot < 0 || idShot >= m_shotItems.size() ) return {};

    return p_mathManager->timeToPos(m_shotItems[idShot]->shot().start); 
}

std::optional<int64_t> ShotManager::getStartTimeOf(int idShot){
    if(idShot < 0 || idShot >= m_shotItems.size() ) return {};

    return m_shotItems[idShot]->shot().start; 
}

const QVector<Shot> ShotManager::shotItemsData() const
{

    QVector<Shot> data; 
    data.reserve(m_shotItems.size());

    for(auto& shotItem : m_shotItems){
        data.push_back(shotItem->shot());
    }
    
    return data;
}

void ShotManager::setShotItemsData(const QVector<Shot> &shots)
{

    clearSelection();

    int shotHeight {40};

    qDeleteAll(m_shotItems);
    m_shotItems.clear();
    qDeleteAll(m_audioShotItems);
    m_audioShotItems.clear();

    for ( auto& IShot : shots ){

        double xPos =  p_mathManager->timeToPos(IShot.start);
        int64_t shotLength = (IShot.end - IShot.start);
        double width = p_mathManager->timeToPos(shotLength);

        ShotItem* shot = new ShotItem(IShot, width, shotHeight);
        AudioShot audioShot{};
        audioShot.start = IShot.start;
        audioShot.end = IShot.end;
        audioShot.title = IShot.title;
        AudioShotItem* audioShotItem = new AudioShotItem(audioShot, width);

        p_scene->addItem(shot);
        p_scene->addItem(audioShotItem);
        shot->setX(xPos);
        audioShotItem->setX(xPos);
        m_shotItems.push_back(shot);
        m_audioShotItems.push_back(audioShotItem);

        requestShotThumbnail(m_shotItems.size()-1, IShot);

    }

}

void ShotManager::createShotItemsFromCuts(const std::vector<int> &cuts)
{

    int shotHeight {40};

    qDeleteAll(m_shotItems);
    m_shotItems.clear();
    qDeleteAll(m_audioShotItems);
    m_audioShotItems.clear();

    int64_t startShot = 0;
    int64_t lengthShot = 0;

    QString baseTitle = PrefManager::instance().getText("shot_detail_title_name");

    for(int i=0; i < cuts.size(); ++i ){

        int64_t nextStartShot = p_mathManager->frameToTime(cuts[i]);
        int64_t endShot = nextStartShot - 1; // la fin du plan = cut - 1 ms
        lengthShot = endShot - startShot;

        double xPos =  p_mathManager->timeToPos(startShot);
        double width = p_mathManager->timeToPos(lengthShot);
        
        Shot shot{baseTitle, startShot, endShot};
        shot.tagImageTime = shot.middle();

        AudioShot audioShot{};
        audioShot.title = baseTitle; audioShot.start = startShot; audioShot.end = endShot;

        ShotItem* shotItem = new ShotItem(shot, width, shotHeight);
        AudioShotItem* audioShotItem = new AudioShotItem(audioShot, width);

        p_scene->addItem(shotItem);
        p_scene->addItem(audioShotItem);
        shotItem->setX(xPos);
        audioShotItem->setX(xPos);
        m_shotItems.push_back(shotItem);
        m_audioShotItems.push_back(audioShotItem);

        requestShotThumbnail(m_shotItems.size()-1, shot);

        startShot = nextStartShot;
    }

    lengthShot = p_mathManager->duration() - startShot;

    double xPos =  p_mathManager->timeToPos(startShot);
    double width = p_mathManager->timeToPos(lengthShot);
    
    Shot shot{baseTitle, startShot, p_mathManager->duration()};
    shot.tagImageTime = shot.middle();
    AudioShot audioShot{};
    audioShot.title = baseTitle; audioShot.start = startShot; audioShot.end = p_mathManager->duration();

    ShotItem* shotItem = new ShotItem(shot, width, shotHeight);
    AudioShotItem* audioShotItem = new AudioShotItem(audioShot, width);
    p_scene->addItem(shotItem);
    p_scene->addItem(audioShotItem);
    shotItem->setX(xPos);
    audioShotItem->setX(xPos);
    m_shotItems.push_back(shotItem);
    m_audioShotItems.push_back(audioShotItem);

    requestShotThumbnail(m_shotItems.size()-1, shot);

    emit shotCountUpdated(shotCount());
}

void ShotManager::requestShotThumbnail(int shotId, const Shot& shot){
    if(p_media->type() != MediaType::Video) return;

    bool displayByTagFrames = PrefManager::instance().getPref("General", "Advanced_timeline_options", "general_timeline_shot_image") == "shot_tag_image";

    if(displayByTagFrames){
        p_thumbnailWorker->requestThumbnail(shotId, shot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
    }else {
        p_thumbnailWorker->requestThumbnail(shotId, shot.start, shot.end - shot.start, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
    }
}

void ShotManager::updateThumbnail(int requestId, QImage image){
    if(requestId < 0 || requestId >= m_shotItems.size()){
        return;
    }

    ShotItem* shotItem = m_shotItems.at(requestId);
    QPixmap pixmap = QPixmap::fromImage(image);
    shotItem->setThumbnail(pixmap);
}
