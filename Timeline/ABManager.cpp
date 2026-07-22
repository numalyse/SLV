#include "Timeline/ABManager.h"
#include "ABManager.h"
#include "PrefManager.h"
#include "SequenceExtractionHelper.h"
#include "Project/ProjectManager.h"

#include <algorithm> 

ABManager::ABManager(QGraphicsScene *scene, TimelineMath *mathManager, QObject *parent)
: BaseRangeManager(scene, mathManager, parent)
{
}

void ABManager::dragMarker(ABMarkerItem* marker, const double pos)
{
    changeMarkerTime(marker, p_mathManager->posToTimeSnapped(pos));
}


std::optional<int64_t> ABManager::getLoopRestartTime(int64_t currentTime)
{
    if(m_markers.size() < 2) {
        m_loopState = LoopState::Idle;
        return {};
    }

    int64_t aTime = m_markers[0]->time();
    int64_t bTime = m_markers[1]->time();

    // on est dans la loop, on met à jour l'état seulement
    if (currentTime >= aTime && currentTime < bTime) {
        m_loopState = LoopState::InLoop;
        return {};
    }

    // undershoot après un restart (InLoop, currentTime < A) : on attend de re-rentrer
    // sans re-seek (sinon set_time(A) re-undershoot -> boucle infinie de seeks)
    if (m_loopState == LoopState::InLoop && currentTime < aTime) {
        return {};
    }

    // on a été dans le loop et on en est sorti (currentTime > B time), on doit restart sur A
    // on ne reset pas l'état à Idle pour le cas ou la vitesse de lecture < 1, pour forcer l'affichage du curseur sur A au lieu de l'undershoot.
    if (m_loopState == LoopState::InLoop && currentTime >= bTime) {
        return aTime;
    }

    // 2 markers mais jamais entré dans la loop (Idle) : on force l'utilisateur dedans.
    m_loopState = LoopState::InLoop;
    return aTime;
}

std::optional<int64_t> ABManager::getDisplayHoldTime(int64_t currentTime) const
{
    if(m_markers.size() < 2) return {};

    int64_t aTime = m_markers[0]->time();

    // quand la vitesse de lecture < 1, libvlc peut renvoyer un temps inférieur à A (undershoot) après un restart.
    // dans ce cas, on veut forcer l'affichage du curseur sur A au lieu de l'undershoot.
    if (m_loopState == LoopState::InLoop && currentTime < aTime) {
        return aTime;
    }

    return {};
}

std::optional<int64_t> ABManager::clampToLoopRange(int64_t time)
{
    if(m_markers.size() < 2) return {}; 

    int64_t aTime = m_markers[0]->time();
    int64_t bTime = m_markers[1]->time();

    return aTime <= bTime ? std::clamp(time, aTime, bTime) : std::clamp(time, bTime, aTime);
}


void ABManager::extractLoop()
{
    if(m_markers.size() < 2) return;

    auto& prefManager = PrefManager::instance();
    auto& projManager = ProjectManager::instance();
    QFileInfo mediaFileInfo(projManager.mediaPath());

    if( ! mediaFileInfo.exists() ) return;

    // if the project is saved in a folder, use it else use prefmanager export path
    QString dialogDir = (projManager.project()->path.isEmpty()) ? prefManager.getPref("Paths", "lp_export") : projManager.project()->path;

    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr,
        prefManager.getText("export_file_path_title"),
        dialogDir + "/" + mediaFileInfo.baseName() + "_"
            + TimeFormatter::fileFormatMsToHHMMSSFF(m_markers[0]->time(), projManager.project()->media->fps())
            + TimeFormatter::fileFormatMsToHHMMSSFF(m_markers[1]->time(), projManager.project()->media->fps())
    );

    selectedPath += '.' + mediaFileInfo.suffix();

    SequenceExtractionHelper *ffmpegProcess = new SequenceExtractionHelper(mediaFileInfo.filePath(), m_markers[0]->time(), m_markers[1]->time());
    connect(ffmpegProcess, &SequenceExtractionHelper::extractionFinished, this, [this, selectedPath, ffmpegProcess](const int exitCode) {
        if (exitCode < 1) {
            emit loopExtractionFailed();
        } else {
            emit loopExtracted(selectedPath);
        }
        ffmpegProcess->deleteLater();
    });
    ffmpegProcess->extractSequence(
        mediaFileInfo.filePath(), m_markers[0]->time(), m_markers[1]->time(), selectedPath
    );



}

void ABManager::cycleMarkers(int64_t time, int markerHeight)
{
    if (m_markers.size() != 1) {
        BaseRangeManager::cycleMarkers(time, markerHeight);
    } else {
        int64_t maxDuration = p_mathManager->duration(); 

        int64_t markerDiff = time - m_markers[0]->time();
        
        int64_t newTime = time;
        if (std::abs(markerDiff) < c_minInterval) {
            newTime = (markerDiff < 0) ? m_markers[0]->time() - c_minInterval : m_markers[0]->time() + c_minInterval;
        }

        newTime = std::clamp(newTime, static_cast<int64_t>(0), maxDuration);

        if (std::abs(newTime - m_markers[0]->time()) < c_minInterval) {
            if (newTime == 0) {
                newTime = m_markers[0]->time() + c_minInterval;
            } else if (newTime == maxDuration) {
                newTime = m_markers[0]->time() - c_minInterval;
            }
        }

        ABMarkerItem* newMarker = new ABMarkerItem(markerHeight, newTime);

        if (newMarker->time() >= m_markers[0]->time()) {
            m_markers.append(newMarker); 
        } else {
            m_markers.insert(0, newMarker);
        }
        
        newMarker->setX(p_mathManager->timeToPos(newTime));
        p_scene->addItem(newMarker);
        emit onPairCompleted();
    }
}

void ABManager::changeMarkerTime(ABMarkerItem *marker, const int64_t time){
    int grabbedMarkerId = m_markers.indexOf(marker);

    if (m_markers.size() == 2) {
        ABMarkerItem* otherMarker = m_markers[m_markers.size() - 1 - grabbedMarkerId];
        int64_t maxDuration = p_mathManager->duration(); 
        
        int64_t newTime = time;

        if (grabbedMarkerId == 0) {
            int64_t upperBound = otherMarker->time() - c_minInterval;  
            upperBound = std::max(static_cast<int64_t>(0), upperBound);
            newTime = std::clamp(time, static_cast<int64_t>(0), upperBound);
            
        } else if (grabbedMarkerId == 1) {
            int64_t lowerBound = otherMarker->time() + c_minInterval;
            lowerBound = std::min(maxDuration, lowerBound);
            newTime = std::clamp(time, lowerBound, maxDuration);
        }

        BaseRangeManager::changeMarkerTime(marker, newTime);
    } else {
        BaseRangeManager::changeMarkerTime(marker, time);
    }
}
