#include "MultiviewVideoCaptureManager.h"
#include "SequenceExtractionHelper.h"
#include "TimeFormatter.h"

void MultiviewVideoCaptureManager::startMultiviewRecord(QVector<Media*>& medias, const QVector<int>& startTimes, const PlayerLayoutArrangement arrangement)
{
    m_extractionsCount = 0;
    m_medias = medias;
    m_startRecordTimes = startTimes;
    m_arrangement = arrangement;
    m_clipsPaths.clear();
    if(startTimes.size() != medias.size() || arrangement == ArrangementUnknown){
        emit multiviewCaptureFailed();
        return;
    }

    // Check if the layout arrangement match with the amount of data received
    switch(arrangement){
    case Arrangement2H:
    case Arrangement2V:
        if(startTimes.size() != 2){
            emit multiviewCaptureFailed();
            return;
        }
        break;
    case Arrangement3H:
    case Arrangement3V:
    case Arrangement3Top:
    case Arrangement3Bot:
    case Arrangement3Left:
    case Arrangement3Right:
        if(startTimes.size() != 3){
            emit multiviewCaptureFailed();
            return;
        }
        break;
    case Arrangement4:
        if(startTimes.size() != 4){
            emit multiviewCaptureFailed();
            return;
        }
        break;
    case Arrangement1:
    case ArrangementUnknown:
        emit multiviewCaptureFailed();
        break;
    }
}

void MultiviewVideoCaptureManager::endMultiviewRecord(const QVector<int> &endTimes, const QString& savePath)
{
    for(int ITime = 0 ; ITime < m_startRecordTimes.size() ; ++ITime){
        if(m_startRecordTimes[ITime] > endTimes[ITime]){
            emit multiviewCaptureFailed();
            return;
        }
    }
    QString saveDir = QFileInfo(savePath).dir().path();
    for(int ITime = 0 ; ITime < m_startRecordTimes.size() ; ++ITime){
        QString timeInterval = TimeFormatter::fileFormatMsToHHMMSSFF(m_startRecordTimes[ITime], m_medias[ITime]->fps()) + '_' + TimeFormatter::fileFormatMsToHHMMSSFF(endTimes[ITime], m_medias[ITime]->fps()) + '.' + m_medias[ITime]->fileExtension();
        QString extractPath = saveDir + '/' + m_medias[ITime]->fileName() + '_' + timeInterval;
        m_clipsPaths.append(extractPath);
        SequenceExtractionHelper *sequenceExtractor = new SequenceExtractionHelper(m_medias[ITime]->filePath(), m_startRecordTimes[ITime], endTimes[ITime]);
        sequenceExtractor->extractSequence(m_medias[ITime]->filePath(), m_startRecordTimes[ITime], endTimes[ITime], extractPath);
        connect(sequenceExtractor, &SequenceExtractionHelper::extractionFinished, this, [this, savePath, endTimes](const int exitCode){
            if(exitCode == -1){
                emit multiviewCaptureFailed();
                return;
            }
            mergeClips(savePath+".mp4", endTimes);
        });
    }
}

void MultiviewVideoCaptureManager::mergeClips(const QString& savePath, const QVector<int> &endTimes)
{
    m_extractionsCount++;
    if(m_extractionsCount != m_medias.size())
        return;

    QProcess *ffmpegMerge = new QProcess();
    QStringList args;

    if(m_medias.size() == 2){
        args << "-i" << m_clipsPaths[0]
             << "-i" << m_clipsPaths[1]
             << "-filter_complex";
        int maxDuration = qMax(endTimes[0]-m_startRecordTimes[0], endTimes[1]-m_startRecordTimes[1]);

        switch(m_arrangement){
        case Arrangement2V:
        {
            int minW = qMin(m_medias[0]->width(),
                            m_medias[1]->width());

            // scale to the same width and tpad to add black screen when the media is finished
            args << QString(
                "[0:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%2ms:color=black[v0];"
                "[1:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%3ms:color=black[v1];"
                "[v0][v1]vstack=inputs=2[v];"
                "[0:a:0][1:a:0]amix=inputs=2:duration=longest[mix]"
            ).arg(minW).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
            .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]));
        }
            break;
        case Arrangement2H:
        {
            int minH = qMin(m_medias[0]->height(),
                            m_medias[1]->height());

            args << QString(
                "[0:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%2ms:color=black[v0];"
                "[1:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%3ms:color=black[v1];"
                "[v0][v1]hstack=inputs=2[v];"
                "[0:a:0][1:a:0]amix=inputs=2:duration=longest[mix]"
            ).arg(minH).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
            .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]));
        }
            break;

        }
        args << "-map" << "[v]"
             << "-map" << "0:a?"
             << "-map" << "1:a?"
             << "-map" << "[mix]"
             << "-c:v" << "libx264"
             << "-c:a" << "aac" << "-b:a" << "192k"
             << "-metadata:s:a:0" << "title=Audio media 1"
             << "-metadata:s:a:1" << "title=Audio media 2"
             << "-metadata:s:a:2" << "title=Audio mixed";
    }
    else if(m_medias.size() == 3){
        args << "-i" << m_clipsPaths[0]
             << "-i" << m_clipsPaths[1]
             << "-i" << m_clipsPaths[2]
             << "-filter_complex";
        int maxDuration = qMax(endTimes[0]-m_startRecordTimes[0], qMax(
            endTimes[1]-m_startRecordTimes[1],
            endTimes[2]-m_startRecordTimes[2])
        );

        switch(m_arrangement){
            case Arrangement3V:
            {
                int minW = qMin(m_medias[0]->width(),qMin(m_medias[1]->width(), m_medias[2]->width()));


                args << QString(
                    "[0:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%2ms:color=black[v0];"
                    "[1:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%3ms:color=black[v1];"
                    "[2:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%4ms:color=black[v2];"
                    "[v0][v1][v2]vstack=inputs=3[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minW).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
            case Arrangement3H:
            {
                int minH = qMin(m_medias[0]->height(),qMin(m_medias[1]->height(), m_medias[2]->height()));

                args << QString(
                    "[0:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%2ms:color=black[v0];"
                    "[1:v]scale-2:%1,tpad=stop_mode=add:stop_duration=%3ms:color=black[v1];"
                    "[2:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%4ms:color=black[v2];"
                    "[v0][v1][v2]hstack=inputs=3[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minH).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
            case Arrangement3Top:
            {
                int minH = qMin(m_medias[1]->height(), m_medias[2]->height());
                int newWidth1 = 2 * qFloor((float(m_medias[1]->width() * minH) / m_medias[1]->height()) / 2);
                int newWidth2 = 2 * qFloor((float(m_medias[2]->width() * minH) / m_medias[2]->height()) / 2);
                int topWidth = newWidth1 + newWidth2;

                // We add black padding to the isolated video to prevent it from taking up too much space
                args << QString(
                    "[0:v]scale=%2:%1:force_original_aspect_ratio=decrease,pad=%2:%1:(ow-iw)/2:(oh-ih)/2:black,tpad=stop_mode=add:stop_duration=%3ms[v0];"
                    "[1:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%4ms[v1];"
                    "[2:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%5ms[v2];"
                    "[v0][v1][v2]xstack=inputs=3:layout=0_0|0_h0|w1_h0[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minH).arg(topWidth).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
            case Arrangement3Bot:
            {
                int minH = qMin(m_medias[0]->height(), m_medias[1]->height());
                int newWidth1 = 2 * qFloor((float(m_medias[0]->width() * minH) / m_medias[0]->height()) / 2);
                int newWidth2 = 2 * qFloor((float(m_medias[1]->width() * minH) / m_medias[1]->height()) / 2);
                int botWidth = newWidth1 + newWidth2;

                args << QString(
                    "[0:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%3ms[v0];"
                    "[1:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%4ms[v1];"
                    "[2:v]scale=%2:%1:force_original_aspect_ratio=decrease,pad=%2:%1:(ow-iw)/2:(oh-ih)/2:black,tpad=stop_mode=add:stop_duration=%5ms[v2];"
                    "[v0][v1][v2]xstack=inputs=3:layout=0_0|w0_0|0_h0:fill=black[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minH).arg(botWidth).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
            case Arrangement3Left:
            {
                int minW = qMin(m_medias[1]->width(), m_medias[2]->width());
                int newHeight1 = 2 * qFloor((float(m_medias[1]->height() * minW) / m_medias[1]->width()) / 2);
                int newHeight2 = 2 * qFloor((float(m_medias[2]->height() * minW) / m_medias[2]->width()) / 2);
                int leftHeight = newHeight1 + newHeight2;

                args << QString(
                    "[0:v]scale=%1:%2:force_original_aspect_ratio=decrease,pad=%1:%2:(ow-iw)/2:(oh-ih)/2:black,tpad=stop_mode=add:stop_duration=%3ms[v0];"
                    "[1:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%4ms[v1];"
                    "[2:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%5ms[v2];"
                    "[v0][v1][v2]xstack=inputs=3:layout=0_0|w0_0|w0_h1:fill=black[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minW).arg(leftHeight).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
            case Arrangement3Right:
            {
                int minW = qMin(m_medias[0]->width(), m_medias[2]->width());
                int newHeight1 = 2 * qFloor((float(m_medias[0]->height() * minW) / m_medias[0]->width()) / 2);
                int newHeight2 = 2 * qFloor((float(m_medias[2]->height() * minW) / m_medias[2]->width()) / 2);
                int rightHeight = newHeight1 + newHeight2;

                args << QString(
                    "[0:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%3ms[v0];"
                    "[1:v]scale=%1:%2:force_original_aspect_ratio=decrease,pad=%1:%2:(ow-iw)/2:(oh-ih)/2:black,tpad=stop_mode=add:stop_duration=%4ms[v1];"
                    "[2:v]scale=%1:-2,tpad=stop_mode=add:stop_duration=%5ms[v2];"
                    "[v0][v1][v2]xstack=inputs=3:layout=0_0|w0_0|0_h0:fill=black[v];"
                    "[0:a:0][1:a:0][2:a:0]amix=inputs=3:duration=longest[mix]"
                ).arg(minW).arg(rightHeight).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
                .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
                .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]));

                break;
            }
        }

        args << "-map" << "[v]"
             << "-map" << "0:a:0?"
             << "-map" << "1:a:0?"
             << "-map" << "2:a:0?"
             << "-map" << "[mix]"
             << "-c:v" << "libx264"
             << "-c:a" << "aac" << "-b:a" << "192k"
             << "-metadata:s:a:0" << "title=Audio media 1"
             << "-metadata:s:a:1" << "title=Audio media 2"
             << "-metadata:s:a:2" << "title=Audio media 3"
             << "-metadata:s:a:3" << "title=Audio mixed";

    }
    else if(m_medias.size() == 4 && m_arrangement == Arrangement4){
        args << "-i" << m_clipsPaths[0]
             << "-i" << m_clipsPaths[1]
             << "-i" << m_clipsPaths[2]
             << "-i" << m_clipsPaths[3]
             << "-filter_complex";
        int maxDuration = qMax(endTimes[0]-m_startRecordTimes[0], qMax(
            endTimes[1]-m_startRecordTimes[1], qMax(
            endTimes[2]-m_startRecordTimes[2],
            endTimes[3]-m_startRecordTimes[3]))
        );


        // To stack 4 medias, we stack the 2 medias on top and 2 medias at the bottom
        // then we stack the top and the bottom together
        int minH1 = qMin(m_medias[0]->height(), m_medias[1]->height());
        int minH2 = qMin(m_medias[2]->height(), m_medias[3]->height());
        int newWidth1 = 2 * qFloor((float(m_medias[0]->width() * minH1) / m_medias[0]->height()) / 2);
        int newWidth2 = 2 * qFloor((float(m_medias[1]->width() * minH1) / m_medias[1]->height()) / 2);
        int newWidth3 = 2 * qFloor((float(m_medias[2]->width() * minH2) / m_medias[2]->height()) / 2);
        int newWidth4 = 2 * qFloor((float(m_medias[3]->width() * minH2) / m_medias[3]->height()) / 2);
        int topWidth = newWidth1 + newWidth2;
        int bottomWidth = newWidth3 + newWidth4;
        int minW = qMax(topWidth, bottomWidth);

        args << QString(
            "[0:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%4ms[v0];"
            "[1:v]scale=-2:%1,tpad=stop_mode=add:stop_duration=%5ms[v1];"
            "[v0][v1]hstack=inputs=2[top];"
            "[2:v]scale=-2:%2,tpad=stop_mode=add:stop_duration=%6ms[v2];"
            "[3:v]scale=-2:%2,tpad=stop_mode=add:stop_duration=%7ms[v3];"
            "[v2][v3]hstack=inputs=2[bot];"
            "[top]scale=%3:-2[topScaled];"
            "[bot]scale=%3:-2[botScaled];"
            "[topScaled][botScaled]vstack=inputs=2[v];"
            "[0:a:0][1:a:0][2:a:0][3:a:0]amix=inputs=4:duration=longest[mix]"
        ).arg(minH1).arg(minH2).arg(minW).arg(maxDuration - (endTimes[0]-m_startRecordTimes[0]))
        .arg(maxDuration - (endTimes[1]-m_startRecordTimes[1]))
        .arg(maxDuration - (endTimes[2]-m_startRecordTimes[2]))
        .arg(maxDuration - (endTimes[3]-m_startRecordTimes[3]));

        args << "-map" << "[v]"
             << "-map" << "0:a:0?"
             << "-map" << "1:a:0?"
             << "-map" << "2:a:0?"
             << "-map" << "3:a:0?"
             << "-map" << "[mix]"
             << "-c:v" << "libx264"
             << "-c:a" << "aac" << "-b:a" << "192k"
             << "-metadata:s:a:0" << "title=Audio media 1"
             << "-metadata:s:a:1" << "title=Audio media 2"
             << "-metadata:s:a:2" << "title=Audio media 3"
             << "-metadata:s:a:3" << "title=Audio media 4"
             << "-metadata:s:a:4" << "title=Audio mixed";
    }

    args << savePath;

    ffmpegMerge->start(SequenceExtractionHelper::getFfmpegPath(), args);
    connect(ffmpegMerge, &QProcess::finished, this, [ffmpegMerge](int exitCode, QProcess::ExitStatus exitStatus){
        if(exitStatus != QProcess::NormalExit || exitCode != 0){
            qDebug() << "Clip merge failed";
            qDebug() << "Exit Status : " << ffmpegMerge->exitStatus() << " exitCode : " << ffmpegMerge->exitCode() << "errors : " << ffmpegMerge->readAllStandardError();
        }
        else
            qDebug() << "Clip merge complete";
    });

}