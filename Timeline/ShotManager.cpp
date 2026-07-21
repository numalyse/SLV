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

// custom selection model, intentionally not using Qt ItemIsSelectable / scene selection:
// - selection is ordered by click order and numbered (drives shot extraction),
//   whereas QGraphicsScene::selectedItems() returns an unordered set;
// - selecting a video shot must also select its paired audio shot (and vice versa),
//   which with scene selection would require reentrancy guards around selectionChanged.
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

    Shot& baseShot = m_shotItems[index]->shot();

    auto oldEnd =  baseShot.end;
    baseShot.end = cutTime - 1; 
    baseShot.tagImageTime = baseShot.middle();
    m_audioShotItems[index]->shot().end = cutTime - 1;

    double newWidth1 = p_mathManager->timeToPos(baseShot.end - baseShot.start );
    m_currentShotItem->setWidth(newWidth1);
    m_audioShotItems[index]->setWidth(newWidth1);

    Shot newShotData =  Shot{ "", cutTime, oldEnd};
    newShotData.tagImageTime = newShotData.middle();

    double pos2 = p_mathManager->timeToPos(newShotData.start);
    double width2 = p_mathManager->timeToPos(newShotData.end) - pos2;

    ShotItem* newShotItem = new ShotItem(newShotData, width2);
    newShotItem->setPos(pos2, m_currentShotItem->y());

    AudioShot newAudioShotData = AudioShot{};
    newAudioShotData.title = "";
    newAudioShotData.start = cutTime;
    newAudioShotData.end = oldEnd;

    AudioShotItem* newAudioShotItem = new AudioShotItem(newAudioShotData, width2);
    newAudioShotItem->setPos(pos2, m_currentShotItem->y());

    // on insère le plan juste apres le plan cut
    m_shotItems.insert(index + 1, newShotItem);
    m_audioShotItems.insert(index + 1, newAudioShotItem);

    if(p_media->type() == MediaType::Video){
        newShotItem->setColorDirty(true);
        m_shotItems[index]->setColorDirty(true);
        p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ShotDetail, -(index + 2), newShotData.tagImageTime, 0, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar());
        p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ShotDetail, -(index + 1), baseShot.tagImageTime, 0, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar());
        if(PrefManager::instance().getPref("General", "Advanced_timeline_options", "general_timeline_shot_image") == "shot_tag_image") {
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, index + 1, newShotData.tagImageTime, 0, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar());
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, index, baseShot.tagImageTime, 0, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar());
        }else {
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, index + 1, newShotData.start, newShotData.end-newShotData.start, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar());
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, index, baseShot.start, baseShot.end - baseShot.start, p_media->filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, p_media->sar()); // update ancienne thumbnail, car si la durée du plan < offset il faut modifier
        }
    }

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

    item->shot().imgTxt += "\n" + m_currentShotItem->shot().imgTxt; // add the note of the current shot to the merged shot
    item->shot().soundTxt += "\n" + m_currentShotItem->shot().soundTxt; // add the note of the current shot to the merged shot

    item->shot().tagImageTime = item->shot().middle(); // updates the tagimage to the middle of the merged shot

    AudioShotItem* audioItem = m_audioShotItems[ShotItemId];
    audioItem->shot().start = item->shot().start;
    audioItem->shot().end = item->shot().end;
    audioItem->shot().imgTxt += "\n" + item->shot().imgTxt; // add the note of the current shot to the merged shot
    audioItem->shot().soundTxt += "\n" + item->shot().soundTxt; // add the note of the current shot to the merged shot

    p_scene->removeItem(m_currentShotItem);
    ShotItem* currentAudioShotItem = m_audioShotItems[m_shotItems.indexOf(m_currentShotItem)];
    p_scene->removeItem(currentAudioShotItem);
    m_shotItems.removeOne(m_currentShotItem);
    m_audioShotItems.removeOne(currentAudioShotItem);
    m_currentShotItem = nullptr;
    
    updateShotItemsPosition();
}

void ShotManager::mergeCurrentWithPrevShot(int64_t cursorTime)
{
    Q_ASSERT( m_currentShotItem );

    int indexOfPrev = m_shotItems.indexOf(m_currentShotItem) - 1;

    mergeCurrentInto(indexOfPrev);
    clearSelection();
    updateCurrentShot(cursorTime);
    emit shotCountUpdated(shotCount());
}

void ShotManager::mergeCurrentWithNextShot(int64_t cursorTime)
{
    Q_ASSERT( m_currentShotItem );

    int indexOfNext = m_shotItems.indexOf(m_currentShotItem) + 1;

    mergeCurrentInto(indexOfNext);
    clearSelection();
    updateCurrentShot(cursorTime);
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
        AudioShotItem* audioShotItem = m_audioShotItems[m_shotItems.indexOf(shotItem)];
        audioShotItem->setX(newXPos);

        newWidth = (shotItem->shot().end - shotItem->shot().start) * p_mathManager->pixelsPerMs();
        shotItem->setWidth(newWidth);
        audioShotItem->setWidth(newWidth);
    }

    p_view->setUpdatesEnabled(true);
    p_scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
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

    qDeleteAll(m_shotItems);
    m_shotItems.clear();
    qDeleteAll(m_audioShotItems);
    m_audioShotItems.clear();

    bool displayByTagFrames = PrefManager::instance().getPref("General", "Advanced_timeline_options", "general_timeline_shot_image") == "shot_tag_image";

    for ( auto& IShot : shots ){

        double xPos =  p_mathManager->timeToPos(IShot.start);
        int64_t shotLength = (IShot.end - IShot.start);
        double width = p_mathManager->timeToPos(shotLength);

        ShotItem* shot = new ShotItem(IShot, width);
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

        if(p_media->type() == MediaType::Video){
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ShotDetail, m_shotItems.size()-1, IShot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            if(displayByTagFrames) {
                p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, IShot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            }else {
                p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, IShot.start, shotLength, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            }
        }

    }

}

void ShotManager::createShotItemsFromCuts(const std::vector<int> &cuts)
{

    qDeleteAll(m_shotItems);
    m_shotItems.clear();
    qDeleteAll(m_audioShotItems);
    m_audioShotItems.clear();

    int64_t startShot = 0;
    int64_t lengthShot = 0;

    bool displayByTagFrames = PrefManager::instance().getPref("General", "Advanced_timeline_options", "general_timeline_shot_image") == "shot_tag_image";

    for(int i=0; i < cuts.size(); ++i ){

        int64_t nextStartShot = p_mathManager->frameToTime(cuts[i]);
        int64_t endShot = nextStartShot - 1; // la fin du plan = cut - 1 ms
        lengthShot = endShot - startShot;

        double xPos =  p_mathManager->timeToPos(startShot);
        double width = p_mathManager->timeToPos(lengthShot);
        
        Shot shot{"", startShot, endShot};
        shot.tagImageTime = shot.middle();

        AudioShot audioShot{};
        audioShot.title = ""; audioShot.start = startShot; audioShot.end = endShot;

        ShotItem* shotItem = new ShotItem(shot, width);
        AudioShotItem* audioShotItem = new AudioShotItem(audioShot, width);

        p_scene->addItem(shotItem);
        p_scene->addItem(audioShotItem);
        shotItem->setX(xPos);
        audioShotItem->setX(xPos);
        m_shotItems.push_back(shotItem);
        m_audioShotItems.push_back(audioShotItem);

        if(p_media->type() == MediaType::Video){
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ShotDetail, m_shotItems.size()-1, shot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            if(displayByTagFrames) {
                p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, shot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            }else {
                p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, shot.start, lengthShot, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
            }
        }

        startShot = nextStartShot;
    }

    lengthShot = p_mathManager->duration() - startShot;

    double xPos =  p_mathManager->timeToPos(startShot);
    double width = p_mathManager->timeToPos(lengthShot);
    
    Shot shot{"", startShot, p_mathManager->duration()};
    shot.tagImageTime = shot.middle();
    AudioShot audioShot{};
    audioShot.title = ""; audioShot.start = startShot; audioShot.end = p_mathManager->duration();

    ShotItem* shotItem = new ShotItem(shot, width);
    AudioShotItem* audioShotItem = new AudioShotItem(audioShot, width);
    p_scene->addItem(shotItem);
    p_scene->addItem(audioShotItem);
    shotItem->setX(xPos);
    audioShotItem->setX(xPos);
    m_shotItems.push_back(shotItem);
    m_audioShotItems.push_back(audioShotItem);

    if(p_media->type() == MediaType::Video){
        p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ShotDetail, m_shotItems.size()-1, shot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
        if(displayByTagFrames) {
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, shot.tagImageTime, 0, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
        }else {
            p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::TimelineShot, m_shotItems.size()-1, shot.start, lengthShot, p_media->filePath(), {m_thumbnailWidth, m_thumbnailHeight}, p_media->sar());
        }
    }

    emit shotCountUpdated(shotCount());
}

QColor getNewBorderColor(const QColor& color)
{
    if (color.valueF() < 0.5) {
        // If the color is too close to dark color, use grey-whitish color for the border
        if (color.valueF() < 0.2) {
            // Merge white color with the original color to get a lighter border color
            QColor whiteColor(255, 255, 255); // White color
            return QColor((color.red() * 0.80 + whiteColor.red() * 0.20),
                          (color.green() * 0.80 + whiteColor.green() * 0.20),
                          (color.blue() * 0.80 + whiteColor.blue() * 0.20));
        }
        return color.lighter(200); // Lighten the color for dark colors
    } else {
        // If the color is too close to light color, use grey-darkish color for the border
        if (color.valueF() > 0.8) {
            // Merge black color with the original color to get a darker border color
            QColor blackColor(0, 0, 0); // Black color
            return QColor((color.red() * 0.80 + blackColor.red() * 0.20),
                          (color.green() * 0.80 + blackColor.green() * 0.20),
                          (color.blue() * 0.80 + blackColor.blue() * 0.20));
        }
        return color.darker(200); // Darken the color for light colors
    }
}

void ShotManager::updateThumbnail(ThumbnailWorker::Requester requester, int requestId, QImage image){
    //if(requester != ThumbnailWorker::Requester::TimelineShot){
    const int shotIndex = requestId < 0 ? -requestId - 1 : requestId;
    if(shotIndex < 0 || shotIndex >= m_shotItems.size()){
        return;
    }

    //if(requestId < 0 || requestId >= m_shotItems.size()){
    ShotItem* shotItem = m_shotItems.at(shotIndex);
    QPixmap pixmap = QPixmap::fromImage(image);

    if(requester == ThumbnailWorker::Requester::TimelineShot){
        shotItem->setThumbnail(pixmap);
        return;
    }

    // ShotItem* shotItem = m_shotItems.at(requestId);
    // QPixmap pixmap = QPixmap::fromImage(image);
    // shotItem->setThumbnail(pixmap);

    if(requester == ThumbnailWorker::Requester::ShotDetail && shotItem->isColorDirty()){
        QColor tagColor = shotItem->getTagImageColor(pixmap);
        if (tagColor.isValid()) {
            shotItem->setColorDirty(false);
            shotItem->shot().color = tagColor;
            shotItem->shot().borderColor = getNewBorderColor(tagColor);
            shotItem->update();
        }
    }
}
