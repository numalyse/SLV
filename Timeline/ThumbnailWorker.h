#ifndef THUMBNAILWORKER_H
#define THUMBNAILWORKER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>

struct ThumbnailRequest{
    int shotId;
    int64_t msStart;
    int64_t shotLength;
};

class ThumbnailWorker : public QThread
{
Q_OBJECT

public:
    explicit ThumbnailWorker(const QString& mediaPath, QObject* parent = nullptr);
    ~ThumbnailWorker();

    void requestThumbnail(int shotId, int64_t msStart, int64_t lenghtMs);
    void stop();

signals:
    void thumbnailReady(int shotId, QImage image);

protected:
    void run() override;

private:
    QString m_videoPath;
    QQueue<ThumbnailRequest> m_queue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_stop = false;
    int m_frameOffset = 100;
    
    cv::Size m_targetSize{100, 75};

};




#endif