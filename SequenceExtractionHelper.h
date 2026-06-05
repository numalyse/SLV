#ifndef SEQUENCEEXTRACTIONHELPER_H
#define SEQUENCEEXTRACTIONHELPER_H

#include "TimeFormatter.h"
#include "SignalManager.h"
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>

//TODO : à mettre dans une classe pour les signaux/barre de progression si possible
class SequenceExtractionHelper : QObject
{

public:
    /// @brief Extract sequence from a media path
    /// @param filePath : media path
    /// @param startTime : start of the sequence from media in ms
    /// @param endTime : end of the sequence from media in ms
    /// @param savePath : path in which the sequence will be saved
    inline static QProcess* extractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath){
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        args << "-ss" << TimeFormatter::msToHHMMSSMilMil(startTime)
             << "-i" << filePath
             << "-t" << TimeFormatter::msToHHMMSSMilMil(endTime - startTime)
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
