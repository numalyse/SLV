#include "ProjectExportThread.h"


ProjectExportThread::ProjectExportThread(ExportType type, const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, QObject *parent)
: QThread(parent), m_type{type}, m_items{items}, m_labels{labels}, m_fps{fps}, m_duration{duration}, m_mediaPath{mediaPath}, m_sar{sar}, m_dst{dstPath}
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
        success = ProjectExportHelper::exportToTxt(m_items, m_labels, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::TagImage:
        success = ProjectExportHelper::exportToTagImage(m_items, m_labels, m_fps, m_duration, m_mediaPath, m_sar, m_dst, progressCallback);
        break;
    case ExportType::PDF:
        success = ProjectExportHelper::exportToPDF(m_items, m_labels, m_fps, m_duration, m_mediaPath, m_sar, m_dst, progressCallback);
        break;
    case ExportType::PPTX:
        success = ProjectExportHelper::exportPython(ExportType::PPTX, m_items, m_labels, m_fps, m_duration, m_mediaPath, m_sar, m_dst, progressCallback);
        break;
    case ExportType::DOCX:
        success = ProjectExportHelper::exportPython(ExportType::DOCX, m_items, m_labels, m_fps, m_duration, m_mediaPath, m_sar, m_dst, progressCallback);
        break;
    case ExportType::CSV:
        success = ProjectExportHelper::exportToCSV(m_items, m_labels, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    case ExportType::SRC:
    case ExportType::MP4:
        success = ProjectExportHelper::exportVideo( m_type, m_items, m_labels, m_fps, m_duration, m_mediaPath, m_sar, m_dst, progressCallback);
        break;
    default:
        break;
    }

    emit exportFinished(success, isInterruptionRequested());
    return;
}
