#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include "TSQueue.h"
#include "Shot.h"

#include <QThread>
#include <QString>
#include <QVector>

#include <optional>

class DecodeThread : public QThread
{
Q_OBJECT
public:
    explicit DecodeThread(
        QString mediaPath, 
        TSQueue<ImgData>* imageQueue,
        const QVector<Shot>& shots, 
        QObject* parent = nullptr, 
        std::optional<int> colorCode = std::nullopt, 
        std::optional<cv::Size> targetSize = std::nullopt
    );

    void decodeTagImages();
    void decodeMedia();

    void run() override;

private:
    QString m_mediaPath{};
    QVector<Shot> m_shots{};

    std::optional<int> m_colorCode{std::nullopt};
    std::optional<cv::Size> m_targetSize{std::nullopt};

    // non onwner => p_
    TSQueue<ImgData>* p_imageQueue = nullptr;

};



#endif