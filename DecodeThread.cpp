#include "DecodeThread.h"
#include "VideoCaptureHelper.h"

#include <opencv2/opencv.hpp>

#include <QDebug>

DecodeThread::DecodeThread(
    QString mediaPath, 
    double sar,
    TSQueue<ImgData>* imageQueue,
    const QVector<Shot> &shots, 
    QObject *parent, 
    std::optional<int> colorCode, 
    std::optional<cv::Size> targetSize
) 
: QThread(parent), m_mediaPath{mediaPath}, m_sar{sar}, p_imageQueue{imageQueue}, m_shots{shots}, m_colorCode{colorCode}, m_targetSize{targetSize}
{
}


void DecodeThread::resizeImage(cv::Mat& src, cv::Mat& dst, cv::InterpolationFlags interpolation)
{

    int origWidth = src.cols;
    int origHeight = src.rows;

    double adjustedWidth = static_cast<double>(origWidth) * m_sar; // prend en compte le pixel aspect ratio pour compenser les pixels rectangulaires

    cv::Size cvTargetSize(m_targetSize.value());

    double scaleWidth = static_cast<double>(cvTargetSize.width) / adjustedWidth;
    double scaleHeight = static_cast<double>(cvTargetSize.height) / origHeight;

    double scale = std::min(scaleWidth, scaleHeight);

    cv::Size newSize(
        static_cast<int>(adjustedWidth * scale),
        static_cast<int>(origHeight * scale)
    );

    cv::resize(src, dst, newSize, 0, 0, interpolation);
}

void DecodeThread::convertImage(cv::Mat& src)
{
    cv::cvtColor(src, src, m_colorCode.value());
}



void DecodeThread::decodeTagImages(){

    cv::VideoCapture cap;
    if (!SLV::openVideoCapture(cap, m_mediaPath, "DecodeThread")) {
        qCritical() << "Opencv : Impossible de d'ouvrir la video " << m_mediaPath;
        p_imageQueue->waitPush({{}, -1, true}); // envoie un stop pour que debloquer le thread qui lit
        return;
    }

    const double fps = cap.get(cv::CAP_PROP_FPS);
    cv::Mat frame;
    cv::Mat processedFrame;
    cv::Mat tempResized;

    for(auto& shot : m_shots){
        SLV::seekToMs(cap, static_cast<double>(shot.tagImageTime), fps);

        if (!cap.read(frame) || frame.empty()) {
            qWarning() << "Impossible de lire la frame au timestamp :" << shot.tagImageTime;
            continue; 
        }

        processedFrame = frame;

        if(m_targetSize.has_value()){
            resizeImage(processedFrame, tempResized, cv::INTER_AREA);
            processedFrame = tempResized;
        }else if(m_sar > 0.0 && m_sar != 1.0 ){
            int origWidth = processedFrame.cols;
            int origHeight = processedFrame.rows;

            cv::Size sarSize(
                static_cast<int>(origWidth * m_sar),
                origHeight
            );

            cv::resize(processedFrame, tempResized, sarSize, 0, 0, cv::INTER_LINEAR);
            processedFrame = tempResized;
        }

        if(m_colorCode.has_value()){
            convertImage(processedFrame);
        }

        p_imageQueue->waitPush({processedFrame.clone(), static_cast<int64_t>(cap.get(cv::CAP_PROP_POS_MSEC)), false});
    }

    p_imageQueue->waitPush({{}, -1, true});
    return;
}

void DecodeThread::decodeMedia(){

    cv::VideoCapture cap;
    if (!SLV::openVideoCapture(cap, m_mediaPath, "DecodeThread")) {
        qCritical() << "Opencv : Impossible de d'ouvrir la video " << m_mediaPath;
        p_imageQueue->waitPush({{}, -1, true}); // envoie un stop pour que debloquer le thread qui lit
        return;
    }

    cv::Mat frame;
    cv::Mat processedFrame;
    cv::Mat tempResized;

    while( cap.read(frame) ){

        if (frame.empty()) {
            qWarning() << "Impossible de lire la frame ";
            continue; 
        }

        processedFrame = frame;

        if(m_targetSize.has_value()){
            resizeImage(processedFrame, tempResized, cv::INTER_NEAREST);
            processedFrame = tempResized;
        }

        if(m_colorCode.has_value()){
            convertImage(processedFrame);
        }

        p_imageQueue->waitPush({processedFrame.clone(), static_cast<int64_t>(cap.get(cv::CAP_PROP_POS_MSEC)), false});
    }
    
    p_imageQueue->waitPush({{}, -1, true});
    return;

}

void DecodeThread::run()
{
    if(!p_imageQueue){
        return;
    } 

    if(m_shots.isEmpty()){
        decodeMedia();
    }else {
        decodeTagImages();
    }
}
