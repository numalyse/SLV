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


void DecodeThread::decodeTagImages(){

    cv::VideoCapture cap(m_mediaPath.toStdString(), cv::CAP_FFMPEG);

    if (!cap.isOpened()) {
        qCritical() << "Impossible de lire la video pour segmenter";
		p_imageQueue->waitPush({{}, true}); // envoie un stop pour que debloquer le thread qui lit
        return;
    }

    cv::Mat frame;

    for(auto& shot : m_shots){
        cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(shot.tagImageTime));
        cap.read(frame);
        p_imageQueue->waitPush({frame.clone(), false});
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
