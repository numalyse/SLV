#include "Timeline/SegmentationThread.h"

#include "DecodeThread.h"
#include "TSQueue.h"
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


void SegmentationThread::run()
{

	std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(10));

	DecodeThread* decodeThread = new DecodeThread(
		m_videoPath, 
		imageQueue.get(), 
		{}, 
		nullptr, 
		std::optional<int>(cv::COLOR_BGR2HLS), 
		std::optional<cv::Size>(m_reducedSize)
	);

	QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
	decodeThread->start();

    cv::VideoCapture cap(m_videoPath.toStdString(), cv::CAP_FFMPEG); 
	// ouverture de la vidéo pour récupérer la durée totale et set la taille du vecteur / pour la progression
 
    if (!cap.isOpened()) {
        qCritical() << "Impossible de lire la video pour segmenter";
		emit segmentationFinished({});
        return;
    }

    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    std::vector<float> frameScores(frameCount);
    frameScores[0] = 0.0;

	if(frameCount<=0){
		emit segmentationFinished({});
		return;
	}

	cap.release();

	cv::Mat prevFrame;
	ImgData imgData;
	
	int currFrameNb = 0;

	int lastPercent = -1;
    while ( true ) {
    
		imageQueue->waitPop(imgData);

		if(isInterruptionRequested()){
			emit segmentationFinished({});
			return;
		}

		if(imgData.isFinished) break;

		int currentPercent = (currFrameNb * 100) / frameCount;
        if (currentPercent > lastPercent) {
            emit progress(currentPercent); 
            lastPercent = currentPercent;
        }

		if( ! prevFrame.empty() ) {
			frameScores[currFrameNb] = computeFrameScore(prevFrame, imgData.img);
		}

		std::swap(prevFrame, imgData.img); // prevFrame devient frame et va etre "overwrite" au prochain tour de boucle évite 
		
		++currFrameNb;

    }

    cap.release();

    std::vector<int> cuts = detectCuts(frameScores);

	qDebug() << "Nombre de plan detectés " << cuts.size() << '\n';

    emit segmentationFinished(cuts);

    return;
}