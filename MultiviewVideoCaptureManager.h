#ifndef MULTIVIEWVIDEOCAPTUREMANAGER_H
#define MULTIVIEWVIDEOCAPTUREMANAGER_H

#include <QObject>

#include "Media.h"
#include "SignalManager.h"

class MultiviewVideoCaptureManager : public QObject
{
    Q_OBJECT
public:
    MultiviewVideoCaptureManager() {};
    void startMultiviewRecord(QVector<Media *> &, const QVector<int>&, const PlayerLayoutArrangement);
    void endMultiviewRecord(const QVector<int>&, const QString&);

public slots:
    void mergeClips(const QString&, const QVector<int> &endTimes);

private:
    QVector<Media*> m_medias;
    QVector<int> m_startRecordTimes = {-1, -1, -1, -1};
    PlayerLayoutArrangement m_arrangement;
    int m_extractionsCount = 0;
    QStringList m_clipsPaths;

signals:
    void multiviewCaptureFailed();
    void multiviewCaptureCompleted();
};

#endif // MULTIVIEWVIDEOCAPTUREMANAGER_H
