#include "ProjectExportThread.h"


ProjectExportThread::ProjectExportThread(ExportType type, const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, QObject *parent)
: QThread(parent), m_type{type}, m_shots{shots}, m_fps{fps}, m_duration{duration}, m_mediaPath{mediaPath}, m_dst{dstPath}
{
}

void ProjectExportThread::run()
{
    int lastPercent = -1; 

    auto progressCallback = [this, &lastPercent](int percent) {
        if (percent != lastPercent) {
            lastPercent = percent;
            emit progress(percent);
        }
    };

    switch (m_type)
    {
    case ExportType::TXT:
        ProjectExportHandler::exportToTxt(m_shots, m_fps, m_duration, m_mediaPath, m_dst, progressCallback);
        break;
    
    default:
        break;
    }

    emit exportFinished(true);
    return;
}
