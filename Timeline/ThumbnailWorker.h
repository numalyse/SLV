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
    /// @param lenghtMs if == 0 will get the frame at msStart else if > 0 will retrieve the frame between msStart and msStart + lenghtMs
    /// @param mediaPath Path of the media
    /// @param targetSize Target size of the thumnails, will use opencv to resize the frame
    void requestThumbnail(int requestId, int64_t msStart, int64_t lengthMs, const QString& mediaPath, QSize targetSize = {100, 75});
    void stop();
    void clearQueue();
    void keepNQueue(const int n);

signals:
    void thumbnailReady(int requestId, const QImage& image);

protected:
    void run() override;

private:
    QQueue<ThumbnailRequest> m_queue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_stop = false;
    int m_frameOffset = 100;
};




#endif
