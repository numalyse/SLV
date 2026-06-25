#ifndef THUMBNAILWORKER_H
#define THUMBNAILWORKER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QImage>
#include <QString>
#include <QSize>
#include <opencv2/opencv.hpp>

struct ThumbnailRequest{
    int requestId;
    int64_t msStart;
    int64_t shotLength;
    QString videoPath;
    QSize targetSize;
    double sar{1.0}; 
};

class ThumbnailWorker : public QThread
{
Q_OBJECT

public:
    explicit ThumbnailWorker(QObject* parent = nullptr);
    ~ThumbnailWorker();

    /// @brief will use opencv to read the frame, converts it to a QImage and then emits a signals so you can retrieve the image
    /// @param requestId Id
    /// @param msStart Frame time in ms
    /// @param lenghtMs if == 0 will get the frame at msStart else if > 0 and > offset, will retrieve the frame at msStart + offset
    /// @param mediaPath Path of the media
    /// @param targetSize Target size of the thumnails, will use opencv to resize the frame
    /// @param sar Sample aspect ratio
    void requestThumbnail(int requestId, int64_t msStart, int64_t lengthMs, const QString& mediaPath, QSize targetSize = {100, 75}, double sar = 1.0);
    void stop();
    void clearQueue();
    void clearQueueForId(int requestId);
    void keepNQueue(const int n);

public slots:
    void requestReleaseCap();

signals:
    void thumbnailReady(int requestId, const QImage& image);

protected:
    void run() override;

private:
    QQueue<ThumbnailRequest> m_queue;
    QQueue<ThumbnailRequest> m_priorityQueue; // requestId < 0 (tagImage du shotDetail), traitée avant m_queue pour rester réactif dans le panneau shotDetail
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_stop = false;
    bool m_releaseCap = false;
    int m_frameOffset = 100;
};




#endif
