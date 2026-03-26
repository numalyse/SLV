#include "Timeline/SegmentationThread.h"

#include "Shot.h"

#include <opencv2/opencv.hpp>

#include <QVector>
#include <QDebug>


SegmentationThread::SegmentationThread(const QString &mediaPath, QObject *parent) : QThread(parent), m_videoPath{mediaPath}
{
}


float SegmentationThread::computeFrameScore(
	const cv::Mat& prevFrameHLS, 
	const cv::Mat& currFrameHLS
) {

	if (prevFrameHLS.channels() != 3 || currFrameHLS.channels() != 3) {
        qDebug() << "les Images n'ont pas 3 channels";
    }

	cv::Mat diff;
	cv::absdiff(prevFrameHLS, currFrameHLS, diff); 

	cv::Scalar means = cv::mean(diff);
	double deltaHue { means[0] };
	double deltaLum { means[1] }; 
	double deltaSat { means[2] };
	// pas de score delta edge pour la performance

	float sumWeights = m_weights[0] + m_weights[1] + m_weights[2];
	float score = (m_weights[0] * deltaHue + m_weights[1] * deltaLum + m_weights[2] * deltaSat) / sumWeights;

	return score;
}

float SegmentationThread::scoreAvg(const std::vector<float>& scores, int currScoreIndex){

	float currScore = scores[currScoreIndex];
	float mean {0.0f};
	int scoreCount {0};

	int startId = (-m_windowWidth + currScoreIndex) < 0 ? 0 : (-m_windowWidth + currScoreIndex);
	int endId = (currScoreIndex + m_windowWidth) > scores.size()-1 ? scores.size()-1 : (currScoreIndex + m_windowWidth);

	for (int windowId = startId ; windowId <= endId ; ++windowId)
	{
		if(windowId == currScoreIndex) continue;

		mean += scores[windowId];
		++scoreCount;
	}

	mean /= scoreCount;

	return currScore / mean ;
	
}

std::vector<int> SegmentationThread::detectCuts(const std::vector<float>& scores){
	
    // window width doit être > 1 

	std::vector<int> cuts;

	for (int IScore{0}; IScore < scores.size(); ++IScore) { 

		float score = scoreAvg(scores, IScore);


		if( score > m_adaptiveThreshold && scores[IScore] > m_minContentVal) {
			cuts.push_back(IScore);
			IScore += m_minSceneLength;
		};
	}

	return cuts;

}

#include <QFile> 
void SegmentationThread::run()
{
    cv::VideoCapture cap(m_videoPath.toStdString(), cv::CAP_FFMPEG);
 
    if (!cap.isOpened()) {
        qCritical() << "Impossible de lire la video pour segmenter";
		emit segmentationFinished({});
        return;
    }

    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    double fps = cap.get(cv::CAP_PROP_FPS); 
    qDebug() << "Total frames: " << frameCount << ", FPS: " << fps;

    std::vector<float> frameScores(frameCount);
    frameScores[0] = 0.0;

	cv::Mat prevFrameHLS;
	cv::Mat frame;
	cv::Mat frameDownscaled;
	cv::Mat frameHLS;
	int currFrameNb = 0;

	int lastPercent = -1;
    while ( cap.read(frame) ) {
        
        if(isInterruptionRequested()){
			emit segmentationFinished({});
            return;
        }

        cv::resize(frame, frameDownscaled, m_reducedSize, 0, 0, cv::INTER_NEAREST);

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


    cap.release();

    std::vector<int> cuts = detectCuts(frameScores);

	qDebug() << "Nombre de plan detectés " << cuts.size() << '\n';

    emit segmentationFinished(cuts);

    return;
}