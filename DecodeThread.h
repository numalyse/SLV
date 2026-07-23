#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include "TSQueue.h"

#include <opencv2/imgproc.hpp>

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
        double sar,
        TSQueue<ImgData>* imageQueue,
        const QVector<int64_t>& imageTimes,
        QObject* parent = nullptr,
        std::optional<int> colorCode = std::nullopt,
        std::optional<cv::Size> targetSize = std::nullopt
    );

    void decodeTagImages();
    void decodeMedia();

    /// @brief Starts to decode the provided media If imageTimes in the constructor was empty, will decode every frame.
    void run() override;

private:

    void resizeImage(cv::Mat &src, cv::Mat &dst, cv::InterpolationFlags interpolation );
    void convertImage(cv::Mat &src);


    QString m_mediaPath{};
    double m_sar{1.0};
    QVector<int64_t> m_imageTimes{};

    std::optional<int> m_colorCode{std::nullopt};
    std::optional<cv::Size> m_targetSize{std::nullopt};

    // non onwner => p_
    TSQueue<ImgData>* p_imageQueue = nullptr;

};



#endif