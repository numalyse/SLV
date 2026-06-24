#ifndef SEQUENCEEXTRACTIONHELPER_H
#define SEQUENCEEXTRACTIONHELPER_H

#include "TimeFormatter.h"
#include "SignalManager.h"
#include "PrefManager.h"

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

//TODO : à mettre dans une classe pour les signaux/barre de progression si possible
class SequenceExtractionHelper : public QObject
{
Q_OBJECT

public:

    enum class ExtractionType{
            AudioOnly,
            Original,
    };

    enum class AudioFormat {
            MP3,
            AAC,
    };

    inline static AudioFormat stringToAudioFormat(const QString& audioFormatString){
            if (audioFormatString == "audio_format_aac") {
                    return AudioFormat::AAC;
            }else {
                    return AudioFormat::MP3;
            }
    }

    inline static QString getFfmpegPath()
    {
        QString appDir = QCoreApplication::applicationDirPath();
        QString ffmpegExe;
#if defined(Q_OS_WIN)
        ffmpegExe = appDir + "/bin/ffmpeg.exe";
#elif defined(Q_OS_MAC)
        ffmpegExe = appDir + "/../Resources/ffmpeg/ffmpeg";
#else
        ffmpegExe = appDir + "/bin/ffmpeg";
#endif
        return ffmpegExe;
    }

    /// @brief Extract sequence from a media path
    /// @param filePath : media path
    /// @param startTime : start of the sequence from media in ms
    /// @param endTime : end of the sequence from media in ms
    /// @param savePath : path in which the sequence will be saved
    inline static QProcess* extractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath, ExtractionType exportType = ExtractionType::Original){
        QProcess *ffmpeg = new QProcess();

        QString finalSavePath = savePath; 
    
        QString prefAudioTypeString = PrefManager::instance().getPref("General", "Exports", "sequence_extraction_audio_format");

        AudioFormat userAudioPreference = stringToAudioFormat(prefAudioTypeString);

        if (exportType == ExtractionType::AudioOnly) {
                QFileInfo fileInfo(savePath);
                QString newExtension;

                switch (userAudioPreference) {
                case AudioFormat::AAC:
                        newExtension = ".m4a";
                        break;
                case AudioFormat::MP3:
                default:
                        newExtension = ".mp3";
                        break;
                }

                finalSavePath = QDir(fileInfo.absolutePath()).filePath(fileInfo.completeBaseName() + newExtension);
        }
                
        
        QStringList args;
        args << "-ss" << TimeFormatter::msToHHMMSSMilMil(startTime)
             << "-i" << filePath
             << "-t" << TimeFormatter::msToHHMMSSMilMil(endTime - startTime);

        switch (exportType)
        {
        case ExtractionType::AudioOnly:
                args << "-vn" << "-map" << "0:a"; 
                if (userAudioPreference == AudioFormat::AAC) {
                        args << "-c:a" << "aac" << "-b:a" << "320k";
                }else {
                        args << "-c:a" << "libmp3lame" << "-q:a" << "2";
                }
                break;
        case ExtractionType::Original:
        default:
                args << "-c" << "copy";
                break;
        }

        args << finalSavePath;

        ffmpeg->start(getFfmpegPath(), args);
        //ffmpeg->start(QString(FFMPEG_EXECUTABLE), args);
        return ffmpeg;
    }

    inline static QProcess* concatenateSequences(const QString& enumSequenceFile, const QString& savePath){
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        args << "-f" << "concat"
             << "-safe" << "0"
             << "-i" << enumSequenceFile
             << "-c" << "copy"
             << savePath;

        ffmpeg->start(getFfmpegPath(), args);
        //ffmpeg->start(QString(FFMPEG_EXECUTABLE), args);

        QProcess::connect(ffmpeg, &QProcess::errorOccurred, &SignalManager::instance(), [](QProcess::ProcessError err){ qDebug() << "PROCESS ERROR : " << err; });
        return ffmpeg;
    }

    inline static QProcess* forceKeyframes(const QString& filePath, int startTime, int endTime, const QString& savePath)
    {
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        // ffmpeg -i a.mp4 -force_key_frames 00:00:09,00:00:12 out.mp4
        args << "-i" << filePath
             << "-force_key_frames" << TimeFormatter::msToHHMMSSMilMil(startTime)+','+TimeFormatter::msToHHMMSSMilMil(endTime)
             << savePath;

        ffmpeg->start(getFfmpegPath(), args);
        return ffmpeg;
    }

    inline void preciseExtractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath, ExtractionType exportType = ExtractionType::Original)
    {
        if(exportType == ExtractionType::AudioOnly){
            extractSequence(filePath, startTime, endTime, savePath, exportType);
            return;
        }

        QFileInfo fi(savePath);
        QString baseName = fi.dir().filePath(fi.completeBaseName());
        QString extention = fi.suffix();
        QString savePathTemp = baseName + "_temp." + extention;
        QString savePathTemp2 = baseName + "_temp_2." + extention;

        if(QFile(savePathTemp).exists() || QFile(savePathTemp2).exists()){
            emit this->extractionFinished(-1);
        }

        int startKeyFrameCut = std::max(startTime - 10000, 0);
        int cutStartTime = startTime - startKeyFrameCut;
        int endKeyFrameCut = endTime + 10000; // ajouter min(end+10s, durationMedia)
        int cutEndTime = cutStartTime + (endTime-startTime);

        // on découpe une séquence grossière pour pouvoir y placer des keyframes sans encoder toute la vidéo
        QProcess *keyFrameCutProcess = extractSequence(filePath, startKeyFrameCut, endKeyFrameCut, savePathTemp);
        connect(keyFrameCutProcess, &QProcess::finished, [this, keyFrameCutProcess, startTime, endTime, cutStartTime, cutEndTime, savePath, savePathTemp, savePathTemp2](){
            // qDebug() << "Exit Status : " << keyFrameCutProcess->exitStatus() << " exitCode : " << keyFrameCutProcess->exitCode() << "errors : " << keyFrameCutProcess->readAllStandardError();
            if (keyFrameCutProcess->exitStatus() == QProcess::NormalExit && keyFrameCutProcess->exitCode() == 0){
                QProcess *addKeyFramesProcess = forceKeyframes(savePathTemp, cutStartTime, cutEndTime, savePathTemp2);
                connect(addKeyFramesProcess, &QProcess::finished, [this, addKeyFramesProcess, startTime, endTime, cutStartTime, cutEndTime, savePath, savePathTemp, savePathTemp2](){
                    // qDebug() << "Exit Status : " << addKeyFramesProcess->exitStatus() << " exitCode : " << addKeyFramesProcess->exitCode() << "errors : " << addKeyFramesProcess->readAllStandardError();
                    if (addKeyFramesProcess->exitStatus() == QProcess::NormalExit && addKeyFramesProcess->exitCode() == 0){
                        qDebug() << "start : " << cutStartTime << " end : " << cutEndTime;
                        QProcess *ffmpeg = extractSequence(savePathTemp2, cutStartTime, cutEndTime, savePath);
                        connect(ffmpeg, &QProcess::finished, [this, ffmpeg, savePathTemp, savePathTemp2](){
                            qDebug() << "Exit Status : " << ffmpeg->exitStatus() << " exitCode : " << ffmpeg->exitCode() << "errors : " << ffmpeg->readAllStandardError();
                            QFile(savePathTemp).remove();
                            QFile(savePathTemp2).remove();
                            emit this->extractionFinished(1);
                        });
                    }else{
                        qDebug() << "error when adding keyframes.";
                        emit this->extractionFinished(-3);
                    }
                });
            }
            else{
                qDebug() << "error when extracting for keyframes.";
                emit this->extractionFinished(-2);
            }
        });
    }

signals:
    void extractionFinished(const int exitCode);

};

#endif // SEQUENCEEXTRACTIONHELPER_H
