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

    /// @brief Extract sequence from a media path
    /// @param filePath : media path
    /// @param startTime : start of the sequence from media in ms
    /// @param endTime : end of the sequence from media in ms
    /// @param savePath : path in which the sequence will be saved
    inline static QProcess* extractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath, ExtractionType exportType = ExtractionType::Original){
        QProcess *ffmpeg = new QProcess();

        QString finalSavePath = savePath; 
    
        QString prefAudioTypeString = PrefManager::instance().getPref("Interface", "Exports", "sequence_extraction_audio_format");

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
        

        QString appDir = QCoreApplication::applicationDirPath();
        QString ffmpegExe;
#if defined(Q_OS_WIN)
        ffmpegExe = appDir + "/bin/ffmpeg.exe";
#elif defined(Q_OS_MAC)
        ffmpegExe = appDir + "/../Resources/ffmpeg/ffmpeg";
    #else
        ffmpegExe = appDir + "/bin/ffmpeg";
#endif

        ffmpeg->start(ffmpegExe, args);
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

                QString appDir = QCoreApplication::applicationDirPath();
        QString ffmpegExe;
#if defined(Q_OS_WIN)
        ffmpegExe = appDir + "/bin/ffmpeg.exe";
#elif defined(Q_OS_MAC)
        ffmpegExe = appDir + "/../Resources/ffmpeg/ffmpeg";
    #else
        ffmpegExe = appDir + "/bin/ffmpeg";
#endif

        ffmpeg->start(ffmpegExe, args);
        //ffmpeg->start(QString(FFMPEG_EXECUTABLE), args);

        QProcess::connect(ffmpeg, &QProcess::errorOccurred, &SignalManager::instance(), [](QProcess::ProcessError err){ qDebug() << "PROCESS ERROR : " << err; });
        return ffmpeg;
    }

};

#endif // SEQUENCEEXTRACTIONHELPER_H
