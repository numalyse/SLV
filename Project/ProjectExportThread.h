#ifndef PROJECTEXPORTTHREAD
#define PROJECTEXPORTTHREAD

#include "Project/ProjectExportHelper.h"
#include "Project/Project.h"
#include "Shot.h"
#include "Media.h"

#include <QThread>
#include <QVector>
#include <QString>

class ProjectExportThread : public QThread
{
Q_OBJECT

public:
    explicit ProjectExportThread(ExportType type, const QVector<ExportItem>& items, const ExportLabels& labels, double fps, int64_t duration, const QString& mediaPath, double sar, const QString& dstPath, QObject* parent = nullptr);
    void run() override;

signals:
    void progress(int);
    void exportFinished(bool success, bool canceled);

private:
    QVector<ExportItem> m_items;
    ExportLabels m_labels;
    QString m_mediaPath;
    double m_sar{1.0};
    QString m_dst;
    int64_t m_duration;
    double m_fps;
    ExportType m_type;

};




#endif // PROJECTEXPORTTHREAD
