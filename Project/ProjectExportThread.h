#ifndef PROJECTEXPORTTHREAD
#define PROJECTEXPORTTHREAD

#include "Project/ProjectExportHelper.h"
#include "Project/Project.h"
#include "Shot.h"

#include <QThread>
#include <QVector>
#include <QString>

class ProjectExportThread : public QThread
{
Q_OBJECT

public:
    explicit ProjectExportThread(ExportType type, const QVector<Shot>& shots, double fps, int64_t duration, const QString& mediaPath, const QString& dstPath, QObject* parent = nullptr);
    void run() override;

signals:
    void progress(int);
    void exportFinished(bool);

private:
    QVector<Shot> m_shots;
    QString m_mediaPath;
    QString m_dst;
    int64_t m_duration;
    double m_fps;
    ExportType m_type;

};




#endif // PROJECTEXPORTTHREAD
