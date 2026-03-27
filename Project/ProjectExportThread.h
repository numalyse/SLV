#ifndef PROJECTEXPORTTHREAD
#define PROJECTEXPORTTHREAD

#include "Project/ProjectExportHandler.h"

#include <QThread>
#include <QString>

class ProjectExportThread : public QThread
{
Q_OBJECT

public:
    explicit ProjectExportThread(ExportType type, const QString& dstPath, QObject* parent = nullptr);
    void run() override;

signals:
    void progress(int);
    void exportFinished(bool);

private:
    QString m_dst;
    ExportType m_type;

};




#endif // PROJECTEXPORTTHREAD
