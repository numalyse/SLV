#include "DecodeThread.h"

#include <opencv2/opencv.hpp>

#include <QDebug>

DecodeThread::DecodeThread(
    QString mediaPath, 
    TSQueue<ImgData>* imageQueue,
    const QVector<Shot> &shots, 
    QObject *parent, 
    std::optional<int> colorCode, 
    std::optional<cv::Size> targetSize
) 
: QThread(parent), m_mediaPath{mediaPath}, p_imageQueue{imageQueue}, m_shots{shots}, m_colorCode{colorCode}, m_targetSize{targetSize}
{
}


void DecodeThread::resizeImage(cv::Mat& src, cv::Mat& dst)
{

    int origWidth = src.cols;
    int origHeight = src.rows;

    cv::Size cvTargetSize(m_targetSize.value());

    double scaleWidth = static_cast<double>(cvTargetSize.width) / origWidth;
    double scaleHeight = static_cast<double>(cvTargetSize.height) / origHeight;

    double scale = std::min(scaleWidth, scaleHeight);

    cv::Size newSize(
        static_cast<int>(origWidth * scale),
        static_cast<int>(origHeight * scale)
    );

    cv::resize(src, dst, newSize, 0, 0, cv::INTER_AREA);
}

void DecodeThread::convertImage(cv::Mat& src)
{
    cv::cvtColor(src, src, m_colorCode.value());
}



void DecodeThread::decodeTagImages(){

    cv::VideoCapture cap(m_mediaPath.toStdString(), cv::CAP_FFMPEG);

    if (!cap.isOpened()) {
        qCritical() << "Impossible de lire la video pour segmenter";
		p_imageQueue->waitPush({{}, true}); // envoie un stop pour que debloquer le thread qui lit
        return;
    }

    cv::Mat frame;
    cv::Mat resized;

    for(auto& shot : m_shots){
        cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(shot.tagImageTime));
        cap.read(frame);

        if (!cap.read(frame) || frame.empty()) {
            qWarning() << "Impossible de lire la frame au timestamp :" << shot.tagImageTime;
            continue; 
        }

        cv::Mat processedFrame = frame;

        if(m_targetSize.has_value()){
            cv::Mat tempResized;
            resizeImage(processedFrame, tempResized);
            processedFrame = tempResized;
        }

        if(m_colorCode.has_value()){
            convertImage(processedFrame);
        }

        p_imageQueue->waitPush({processedFrame.clone(), false});
    }

    p_imageQueue->waitPush({{}, true});
    return;
}

void DecodeThread::decodeMedia(){
/* 
    cv::VideoCapture cap(m_mediaPath.toStdString(), cv::CAP_FFMPEG);

	cv::Mat frame;

     while ( cap.read(frame) ) {
        
        if(isInterruptionRequested()){
			emit segmentationFinished({});
            return;
        }

        cv::resize(frame, frameDownscaled, m_reducedSize, 0, 0, cv::INTER_AREA);

		cv::cvtColor(frameDownscaled, frameHLS, cv::COLOR_BGR2HLS);

		if( ! prevFrameHLS.empty() ) {
			frameScores[currFrameNb] = computeFrameScore(prevFrameHLS, frameHLS);
		}

		std::swap(prevFrameHLS, frameHLS); // prevFrame devient frame et va etre "overwrite" au prochain tour de boucle évite 
		++currFrameNb;
		int currentPercent = (currFrameNb * 100) / frameCount;

        if (currentPercent > lastPercent) {
            emit progress(currentPercent); 
            lastPercent = currentPercent;
        }
    } 
*/

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
