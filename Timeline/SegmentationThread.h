#ifndef SEGMENTATIONTHREAD_H
#define SEGMENTATIONTHREAD_H

#include "Shot.h"

#include <opencv2/opencv.hpp>

#include <QString>
#include <QThread>

#include <array>

class SegmentationThread : public QThread
{
Q_OBJECT

public:
    explicit SegmentationThread(const QString& mediaPath, QObject* parent = nullptr);
    void run() override;

signals:
    void progress(int);
    void segmentationFinished( const std::vector<int> cuts );
    void errorOccured(const QString& errorMsg);

private:
    float computeFrameScore(const cv::Mat &prevFrameHLS, const cv::Mat &currFrameHLS);

    float scoreAvg(const std::vector<float> &scores, int currScoreIndex);

    std::vector<int> detectCuts(const std::vector<float> &scores);
    
    QString m_videoPath;
    cv::Size m_reducedSize {256, 256};
    std::vector<float> m_weights {1.0f, 1.0f, 1.0f};
    int m_windowWidth = 2;
    float m_adaptiveThreshold = 3.0;
	int m_minSceneLength = 15;
    float m_minContentVal = 15.0f;

};




#endif