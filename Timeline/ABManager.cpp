#include "Timeline/ABManager.h"
#include "ABManager.h"
#include "PrefManager.h"
#include "SequenceExtractionHelper.h"
#include "Project/ProjectManager.h"

ABManager::ABManager(QGraphicsScene *scene, TimelineMath *mathManager, QObject *parent)
: BaseRangeManager(scene, mathManager, parent)
{
}


std::optional<int64_t> ABManager::getLoopRestartTime(int64_t currentTime)
{
    if(m_markers.size() < 2) return {};

    int64_t aTime = m_markers[0]->time();
    int64_t bTime = m_markers[1]->time();

    // marge, apres avoir restart la loop, currentTime peut etre legerement inférieur à aTime, on ajoute une frame de marge pour ne pas restart en boucle
    int64_t margin = 1000.0 / p_mathManager->fps();

    if (currentTime >= bTime || currentTime < aTime - margin) {
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
    QString dialogDir = (projManager.projet()->path.isEmpty()) ? prefManager.getPref("Paths", "lp_export") : projManager.projet()->path;

    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr,
        prefManager.getText("export_file_path_title"),
        dialogDir + "/" + mediaFileInfo.baseName() + "_"
            + TimeFormatter::fileFormatMsToHHMMSSFF(m_markers[0]->time(), projManager.projet()->media->fps())
            + TimeFormatter::fileFormatMsToHHMMSSFF(m_markers[1]->time(), projManager.projet()->media->fps())
    );

    selectedPath += '.' + mediaFileInfo.suffix();

    QProcess* ffmpegProcess = SequenceExtractionHelper::extractSequence(
        mediaFileInfo.filePath(), 
        m_markers[0]->time(), 
        m_markers[1]->time(), 
        selectedPath
    );

    connect(ffmpegProcess, &QProcess::finished, this, [this, selectedPath, ffmpegProcess](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode != 0) {
            emit loopExtractionFailed();
        } else {
            emit loopExtracted(selectedPath);
        }

        ffmpegProcess->deleteLater();
    });

}
