#ifndef SEQUENCEEXTRACTIONHELPER_H
#define SEQUENCEEXTRACTIONHELPER_H

#include "TimeFormatter.h"
#include "SignalManager.h"
#include <QString>
#include <QStringList>
#include <QProcess>
#include <qdebug.h>

//TODO : à mettre dans une classe pour les signaux/barre de progression si possible
namespace SequenceExtractionHelper
{

    /// @brief Extract sequence from a media path
    /// @param filePath : media path
    /// @param startTime : start of the sequence from media in ms
    /// @param endTime : end of the sequence from media in ms
    /// @param savePath : path in which the sequence will be saved
    inline void extractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath){
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        args << "-ss" << TimeFormatter::msToHHMMSSMilMil(startTime)
             << "-i" << filePath
             << "-t" << TimeFormatter::msToHHMMSSMilMil(endTime - startTime)
             << "-c" << "copy"
             << savePath;

        ffmpeg->start("ffmpeg", args);
    }

    inline void concatenateSequences(const QString& enumSequenceFile, const QString& savePath){
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        args << "-f" << "concat"
             << "-safe" << "0"
             << "-i" << enumSequenceFile
             << "-c" << "copy"
             << savePath;

        ffmpeg->start("ffmpeg", args);
        QProcess::connect(ffmpeg, &QProcess::errorOccurred, &SignalManager::instance(), [](QProcess::ProcessError err){ qDebug() << "PROCESS ERROR : " << err; });
    }

} // namespace

#endif // SEQUENCEEXTRACTIONHELPER_H
