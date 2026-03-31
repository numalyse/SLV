#include "Timeline/SegmentationThread.h"

#include "DecodeThread.h"
#include "TSQueue.h"
#include "Shot.h"

#include <opencv2/opencv.hpp>

#include <QVector>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>


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

	QProcess pythonProcess;
    pythonProcess.setProcessChannelMode(QProcess::MergedChannels);

    QStringList arguments;
    arguments << "segmentation.py" << m_videoPath;
    arguments << ("1"); 

    pythonProcess.start("py", arguments);

    if (!pythonProcess.waitForStarted()) {
        qCritical() << "Impossible de lancer le script Python.";
        emit segmentationFinished({});
        return;
    }

    std::vector<int> finalCuts;

    while (pythonProcess.waitForReadyRead(-1)) {
        while (pythonProcess.canReadLine()) {
            QString line = QString::fromUtf8(pythonProcess.readLine()).trimmed();

            // Gestion de l'interruption utilisateur
            if (isInterruptionRequested()) {
                pythonProcess.kill();
                emit segmentationFinished({});
                return;
            }

            if (line.startsWith("PROGRESS:")) {
                int percent = line.mid(9).toInt();
                emit progress(percent); 
            } 
            else if (line.startsWith("RESULT:")) {
                // Parsing du JSON reçu
                QString jsonStr = line.mid(7);
                QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
                QJsonArray arr = doc.array();
                
                for (const QJsonValue& val : arr) {
                    finalCuts.push_back(val.toInt());
                }
            } 
            else {
                qDebug() << "[PYTHON LOG]:" << line;
            }
        }
    }

    pythonProcess.waitForFinished();

    if (pythonProcess.exitCode() != 0) {
        qWarning() << "Le script Python s'est terminé avec une erreur.";
    }

    emit segmentationFinished(finalCuts);
}