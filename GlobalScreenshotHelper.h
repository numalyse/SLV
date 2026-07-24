#ifndef GLOBALSCREENSHOTHELPER_H
#define GLOBALSCREENSHOTHELPER_H

#include "TimeFormatter.h"
#include "PlayerLayoutManager.h"
#include "PrefManager.h"
#include "VideoCaptureHelper.h"

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QMessageBox>

struct GlobalScreenshotPlayerData{
    QString mediaPath;
    int currentTime;
    double sar{1.0};
};

class GlobalScreenshotHelper : public QThread
{
Q_OBJECT
public:
    const QList<GlobalScreenshotPlayerData> m_playersData;
    const PlayerLayoutArrangement arrangement;

    inline GlobalScreenshotHelper(const QList<GlobalScreenshotPlayerData>& playersData, const PlayerLayoutArrangement arr) : m_playersData(playersData), arrangement(arr){};
    inline void run() override{
        takeGlobalScreenshot();
    };

signals:
    void finishedSuccess(const QString& mergedPath);
    void finishedError();

private:

    inline void takeGlobalScreenshot(){
        qDebug() << "Global Screenshot";
        auto& prefManager = PrefManager::instance();
        QList<cv::Mat> screenshots;
        QList<cv::Mat> resizedScreenshots;
        cv::VideoCapture cap;
        int minWidth = 10000000;
        int minHeight = 10000000;
        QString mergedPath(prefManager.getPref("Paths", "screenshot") + '/');
        if(!QDir(mergedPath).exists()) QDir().mkdir(mergedPath);

        GlobalScreenshotPlayerData currPlayerData;

        for(size_t IPlayerData = 0; IPlayerData < m_playersData.size(); ++IPlayerData){
            cap.release();
            currPlayerData = m_playersData[IPlayerData];

            if (!SLV::openVideoCapture(cap, currPlayerData.mediaPath, "Global Screenshot")) {
                qCritical() << "Opencv : Impossible de d'ouvrir la video pour extraire les une image " << currPlayerData.mediaPath;
                emit finishedError();
                return;
            }

            cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(currPlayerData.currentTime));
            cv::Mat playerFrame;
            cv::Mat playerFrameResized; // pour prendre en compte le sar
            if(!cap.read(playerFrame)){
                qDebug() << "Erreur dans le chargement du screenshot de " + currPlayerData.mediaPath;
                emit finishedError();
                return;
            }

            QString path = QFileInfo(currPlayerData.mediaPath).baseName();
            double fps = cap.get(cv::CAP_PROP_FPS);
            QByteArray pathBytes = path.toUtf8();

            QString fullOutputPath = prefManager.getPref("Paths", "screenshot")
                                    + '/'
                                    + pathBytes.constData() + "_"
                                    + TimeFormatter::fileFormatMsToHHMMSSFF(currPlayerData.currentTime, fps)
                                    + ".png";

            int origWidth = playerFrame.cols;
            int origHeight = playerFrame.rows;
            double sar = currPlayerData.sar;

            int adjustedWidth = (sar > 0) ? origWidth * sar : origWidth;

            cv::Size newSize(adjustedWidth, origHeight);
            cv::resize(playerFrame, playerFrameResized, newSize, 0, 0, cv::INTER_AREA);

            screenshots.append(playerFrameResized);

            if(!cv::imwrite(fullOutputPath.toStdString(), playerFrameResized)){
                qDebug() << "Erreur dans l'enregistrement de la capture multiple";
                emit finishedError();
                return;
            }
            mergedPath += path.left(std::min(5, int(path.size()))) + '_'
                        + TimeFormatter::fileFormatMsToHHMMSSFF(currPlayerData.currentTime, fps) 
                        + (IPlayerData != m_playersData.size()-1 ? "_" : "");
                        
            minWidth = std::min(minWidth, playerFrame.cols);
            minHeight = std::min(minHeight, playerFrame.rows);
        }

        for(size_t IPlayer = 0; IPlayer < screenshots.size(); ++IPlayer){
            int origWidth = screenshots[IPlayer].cols;
            int origHeight = screenshots[IPlayer].rows;
            int newWidth = -1;
            int newHeight = -1;

            float imgRatio = float(origWidth) / float(origHeight);
            if(imgRatio > 1){
                newWidth = minWidth;
                newHeight = int(newWidth / imgRatio);
            }
            else{
                newHeight = minHeight;
                newWidth = int(newHeight * imgRatio);
            }

            cv::Size newSize(newWidth, newHeight);
            resizedScreenshots.resize(screenshots.size());
            cv::resize(screenshots[IPlayer], resizedScreenshots[IPlayer], newSize, 0, 0, cv::INTER_AREA);

        }
        cv::Mat mergedScreenshot;
        cv::Mat tempImage;
        QList<cv::Mat> tempImgList;
        switch(arrangement){

        case Arrangement1:
            return;
        case Arrangement2H:
            Q_ASSERT(resizedScreenshots.size() == 2);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 0);
            cv::hconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            break;
        case Arrangement2V:
            Q_ASSERT(resizedScreenshots.size() == 2);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 1);
            cv::vconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            break;
        case Arrangement3H:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 0);
            cv::hconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            addBlackPadding(mergedScreenshot, resizedScreenshots[2], 0);
            cv::hconcat(mergedScreenshot, resizedScreenshots[2], mergedScreenshot);
            break;
        case Arrangement3V:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 1);
            cv::vconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            addBlackPadding(mergedScreenshot, resizedScreenshots[2], 1);
            cv::vconcat(mergedScreenshot, resizedScreenshots[2], mergedScreenshot);
            break;
        case Arrangement3Top:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[1], resizedScreenshots[2], 0);
            cv::hconcat(resizedScreenshots[1], resizedScreenshots[2], mergedScreenshot);
            addBlackPadding(resizedScreenshots[0], mergedScreenshot, 1);
            cv::vconcat(resizedScreenshots[0], mergedScreenshot, mergedScreenshot);
            break;
        case Arrangement3Bot:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 0);
            cv::hconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            addBlackPadding(mergedScreenshot, resizedScreenshots[2], 1);
            cv::vconcat(mergedScreenshot, resizedScreenshots[2], mergedScreenshot);
            break;
        case Arrangement3Left:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[1], resizedScreenshots[2], 1);
            cv::vconcat(resizedScreenshots[1], resizedScreenshots[2], mergedScreenshot);
            addBlackPadding(resizedScreenshots[0], mergedScreenshot, 0);
            cv::hconcat(resizedScreenshots[0], mergedScreenshot, mergedScreenshot);
            break;
        case Arrangement3Right:
            Q_ASSERT(resizedScreenshots.size() == 3);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[2], 1);
            cv::vconcat(resizedScreenshots[0], resizedScreenshots[2], mergedScreenshot);
            addBlackPadding(mergedScreenshot, resizedScreenshots[1], 0);
            cv::hconcat(mergedScreenshot, resizedScreenshots[1], mergedScreenshot);
            break;
        case Arrangement4:
            Q_ASSERT(resizedScreenshots.size() == 4);
            addBlackPadding(resizedScreenshots[0], resizedScreenshots[1], 0);
            cv::hconcat(resizedScreenshots[0], resizedScreenshots[1], mergedScreenshot);
            addBlackPadding(resizedScreenshots[2], resizedScreenshots[3], 0);
            cv::hconcat(resizedScreenshots[2], resizedScreenshots[3], tempImage);
            addBlackPadding(mergedScreenshot, tempImage, 1);
            cv::vconcat(mergedScreenshot, tempImage, mergedScreenshot);
            break;
        case ArrangementUnknown:
            return;
        }

        mergedPath += ".png";
        qDebug() << "final mergedPath : " << mergedPath;
        cv::imwrite(mergedPath.toUtf8().constData(), mergedScreenshot);
        emit finishedSuccess(mergedPath);
    }

    /// @brief Adds black padding to the smaller images in matList.
    /// @param matList : list of images
    /// @param orientation : 0 = Adds top and bottom padding, 1 : Adds left and right padding
    inline static void addBlackPadding(cv::Mat& img1, cv::Mat& img2, int orientation)
    {
        if (orientation == 0) {
            int targetHeight = std::max(img1.rows, img2.rows);

            int top1 = (targetHeight - img1.rows) / 2;
            int bottom1 = targetHeight - img1.rows - top1;

            int top2 = (targetHeight - img2.rows) / 2;
            int bottom2 = targetHeight - img2.rows - top2;

            cv::copyMakeBorder(img1, img1, top1, bottom1, 0, 0, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
            cv::copyMakeBorder(img2, img2, top2, bottom2, 0, 0, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
        }
        else {
            int targetWidth = std::max(img1.cols, img2.cols);

            int left1 = (targetWidth - img1.cols) / 2;
            int right1 = targetWidth - img1.cols - left1;

            int left2 = (targetWidth - img2.cols) / 2;
            int right2 = targetWidth - img2.cols - left2;

            cv::copyMakeBorder(img1, img1, 0, 0, left1, right1, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
            cv::copyMakeBorder(img2, img2, 0, 0, left2, right2, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
        }
    }

};

#endif // GLOBALSCREENSHOTHELPER_H
