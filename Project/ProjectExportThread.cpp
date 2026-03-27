#include "ProjectExportThread.h"


ProjectExportThread::ProjectExportThread(ExportType type, const QString &dstPath, QObject *parent)
: QThread(parent), m_type{type}, m_dst{dstPath}
{

}

void ProjectExportThread::run()
{
}
