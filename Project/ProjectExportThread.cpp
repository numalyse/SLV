#include "ProjectExportThread.h"


ProjectExportThread::ProjectExportThread(ExportType type, const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, QObject *parent)
: QThread(parent), m_type{type}, m_shots{shots}, m_fps{fps}, m_duration{duration}, m_mediaPath{mediaPath}, m_dst{dstPath}
{
}

void ProjectExportThread::run()
{
    int lastPercent = -1; 
    bool success = false;

    auto progressCallback = [this, &lastPercent](int percent) -> bool {
        if (this->isInterruptionRequested()) {
            return false; 
        }

        if (percent != lastPercent) {
            lastPercent = percent;
            emit progress(percent);
        }
        return true;
    };

    switch (m_type)
    {
    case ExportType::TXT:
        success = ProjectExportHelper::exportToTxt(m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::TagImage:
        success = ProjectExportHelper::exportToTagImage(m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::PDF:
        success = ProjectExportHelper::exportToPDF(m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::PPTX:
        success = ProjectExportHelper::exportPython(ExportType::PPTX, m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::DOCX:
        success = ProjectExportHelper::exportPython(ExportType::DOCX, m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::SRC:
    case ExportType::MP4:
        success = ProjectExportHelper::exportVideo( m_type, m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    default:
        break;
    }

    emit exportFinished(success);
    return;
}
