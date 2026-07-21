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
#include <QTemporaryDir>
#include <QThreadPool>


/// @brief Struct to store different keyframes of a video
/// BS = first frame before the start timecode
/// AS = first frame after the start timecode
/// BE = first frame before the end timecode
/// BS = first frame after the end timecode
struct KeyframeBounds {
    int64_t BS = 0;
    int64_t AS = -1;
    int64_t BE = 0;
    int64_t AE = -1;
};

struct CodecParams {
    QString codecName;
    QString profile;
    QString level;
    QString pixFmt;
    bool valid = false;
};

class SequenceExtractionHelper : public QObject
{
Q_OBJECT

private:

    const QString m_videoPath;
    const int m_startTime;
    const int m_endTime;
    int m_progression;
    QTemporaryDir m_tempDir;
    QFile *m_segmentList = nullptr;
    KeyframeBounds m_kb;
    CodecParams m_codecParams;

public:

    SequenceExtractionHelper(const QString& filePath, const int start, const int end) : m_videoPath(filePath), m_startTime(start), m_endTime(end), m_progression(0)
    {}

    ~SequenceExtractionHelper()
    {
        m_tempDir.remove();
    }

    enum class ExtractionType{
        AudioOnly,
        Original,
        Lossless,
        Reencode
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

    /// @brief Extract sequence from a media path (can have artefacts)
    /// @param filePath : media path
    /// @param startTime : start of the sequence from media in ms
    /// @param endTime : end of the sequence from media in ms
    /// @param savePath : path in which the sequence will be saved
    inline QProcess* extractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath, ExtractionType exportType = ExtractionType::Original, bool finishSignal = true, unsigned int audioTrack = 0){
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

        int duration = endTime-startTime;
        QStringList args;
        args << "-ss" << QString::number(startTime)+"ms"
             << "-i" << filePath
             << "-t" << QString::number(duration)+"ms";

        switch (exportType)
        {
        case ExtractionType::AudioOnly:
            if (userAudioPreference == AudioFormat::AAC) {
                args << "-vn" << "-map" << "0:a"
                     << "-c:a" << "aac" << "-b:a" << "320k";
            }else {
                args << "-vn" << "-map" << "0:a:"+QString::number(audioTrack)
                     << "-c:a" << "libmp3lame" << "-q:a" << "2";
            }
            break;
        case ExtractionType::Original:
        default:
            args << "-map" << "0:v"
                 << "-map" << "0:a?"
                 << "-map" << "0:s?"
                 << "-c" << "copy";
                 // << "-progress" << "pipe:1"
                 // << "-nostats";
            break;
        }

        args << finalSavePath;

        connect(ffmpeg, &QProcess::finished, [this, ffmpeg, finishSignal](){
            if (ffmpeg->exitStatus() != QProcess::NormalExit || ffmpeg->exitCode() != 0){
                qDebug() << "Exit Status : " << ffmpeg->exitStatus() << " exitCode : " << ffmpeg->exitCode() << "errors : " << ffmpeg->readAllStandardError();
            }
            if(!finishSignal) return;
            if (ffmpeg->exitStatus() != QProcess::NormalExit || ffmpeg->exitCode() != 0)
                emit extractionFinished(-1);
            emit extractionFinished(1);

        });
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

    inline static QProcess* processForceKeyframes(const QString& filePath, QVector<int> keyframesTimecode, const QString& savePath, const CodecParams &codecParams)
    {
        QProcess *ffmpeg = new QProcess();
        QStringList args;
        QString keyframesString = "";
        for(size_t Ikf = 0; Ikf < keyframesTimecode.size(); ++Ikf){
            keyframesString += QString::number(keyframesTimecode[Ikf]) + "ms" + (Ikf != keyframesTimecode.size()-1 ? "," : "");
        }

        args << "-i" << filePath
             << "-force_key_frames" << keyframesString
             << "-map" << "0:v:0" // copy video tracks
             << "-map" << "0:a?" // copy audio tracks if exist
             << "-map" << "0:s?" // copy subtitles tracks if exist
             << "-map" << "0:t?"; // copy ass subtitles if exist

        // If codecParams are valid, keep them for extraction. This part should never be accessed
        if (codecParams.valid) {
            args << "-c:v" << codecParams.codecName
                 << "-profile:v" << codecParams.profile
                 << "-level:v" << codecParams.level
                 << "-pix_fmt" << codecParams.pixFmt
                 << "-bf" << "0"
                 << "-flags" << "+cgop";
        } else {
            args << "-c:v" << "libx264"; // Let ffmpeg chose the encoder
        }

        args << "-c:a" << "copy"
             << "-c:s" << "copy"
             << "-c:t" << "copy"
             << savePath;

        qDebug() << args.join(" ");
        ffmpeg->start(getFfmpegPath(), args);
        return ffmpeg;
    }

    // inline static bool findKeyframeBounds(const QString &filePath, int64_t startMs, int64_t endMs, KeyframeBounds &kb)
    // {
    //     // Initialize video
    //     AVFormatContext *in_fmt = nullptr;
    //     if (avformat_open_input(&in_fmt, filePath.toUtf8().constData(), nullptr, nullptr) < 0)
    //         return false;
    //     if (avformat_find_stream_info(in_fmt, nullptr) < 0) {
    //         avformat_close_input(&in_fmt);
    //         return false;
    //     }

    //     int videoStreamIndex = av_find_best_stream(in_fmt, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    //     if (videoStreamIndex < 0) {
    //         avformat_close_input(&in_fmt);
    //         return false;
    //     }
    //     AVStream *stream = in_fmt->streams[videoStreamIndex];

    //     AVPacket *pkt = av_packet_alloc();
    //     kb = KeyframeBounds{};

    //     // Retrieve keyframes
    //     while (av_read_frame(in_fmt, pkt) >= 0) {
    //         if (pkt->stream_index == videoStreamIndex &&
    //             (pkt->flags & AV_PKT_FLAG_KEY) && pkt->pts != AV_NOPTS_VALUE)
    //         {
    //             int64_t ms = llround(pkt->pts * av_q2d(stream->time_base) * 1000.0);
    //             if (ms <= startMs) kb.BS = ms;
    //             if (ms >= startMs && kb.AS == -1) kb.AS = ms;
    //             if (ms <= endMs) kb.BE = ms;
    //             if (ms >= endMs && kb.AE == -1) kb.AE = ms;
    //         }
    //         av_packet_unref(pkt);
    //     }
    //     av_packet_free(&pkt);
    //     avformat_close_input(&in_fmt);

    //     if (kb.AE == -1) kb.AE = endMs;
    //     if (kb.AS == -1) kb.AS = kb.BS;

    //     return true;
    // }

    // inline static CodecParams getVideoCodecParams(const QString &filePath)
    // {
    //     CodecParams params;
    //     AVFormatContext *fmt = nullptr;
    //     if (avformat_open_input(&fmt, filePath.toUtf8().constData(), nullptr, nullptr) < 0)
    //         return params;
    //     if (avformat_find_stream_info(fmt, nullptr) < 0) {
    //         avformat_close_input(&fmt);
    //         return params;
    //     }

    //     int videoStreamIndex = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    //     if (videoStreamIndex < 0) {
    //         avformat_close_input(&fmt);
    //         return params;
    //     }

    //     AVCodecParameters *codecpar = fmt->streams[videoStreamIndex]->codecpar;

    //     // Mapping codec_id -> nom d'encodeur ffmpeg (à étendre si besoin)
    //     switch (codecpar->codec_id) {
    //     case AV_CODEC_ID_H264:
    //         params.codecName = "libx264";
    //         break;
    //     case AV_CODEC_ID_HEVC:
    //         params.codecName = "libx265";
    //         break;
    //     default:
    //         // Codec non géré explicitement : on laisse ffmpeg décider,
    //         // mais dans ce cas le fix "profile/level" ne s'appliquera pas
    //         params.codecName = QString();
    //         break;
    //     }

    //     const char *profileName = av_get_profile_name(avcodec_find_decoder(codecpar->codec_id), codecpar->profile);
    //     if (profileName) {
    //         QString rawProfile = QString(profileName).toLower();
    //         params.profile = rawProfile.remove(' ');
    //     }

    //     if (codecpar->level > 0) {
    //         switch (codecpar->codec_id) {
    //         case AV_CODEC_ID_HEVC:
    //             // HEVC stocke general_level_idc = level * 30
    //             params.level = QString::number(codecpar->level / 30.0, 'f', 1);
    //             break;
    //         case AV_CODEC_ID_H264:
    //         default:
    //             // H.264 stocke level * 10
    //             params.level = QString::number(codecpar->level / 10.0, 'f', 1);
    //             break;
    //         }
    //     }

    //     const char *pixFmtName = av_get_pix_fmt_name((AVPixelFormat)codecpar->format);
    //     if (pixFmtName)
    //         params.pixFmt = QString(pixFmtName);

    //     params.valid = !params.codecName.isEmpty() && !params.profile.isEmpty() && !params.pixFmt.isEmpty();

    //     avformat_close_input(&fmt);
    //     return params;
    // }

    /// @brief Extract, force keyframe at startTime and extract again
    inline void reencodeTimecode(const bool isStart, const int BT, const int AT)
    {
        QString extractionPath1 = m_tempDir.path() + "/extract1_" + (isStart ? "start" : "end") + "." + QFileInfo(m_videoPath).suffix();

        // First extraction, to avoid reencoding the entire video
        QProcess *extract1 = extractSequence(m_videoPath, BT, AT, extractionPath1, ExtractionType::Original, false);
        connect(extract1, &QProcess::finished, [this, extract1, isStart, extractionPath1, BT, AT](){
            if (extract1->exitStatus() != QProcess::NormalExit || extract1->exitCode() != 0)
                qDebug() << "[Sequence extraction] Error in first extraction for " + QString(isStart ? "start" : "end");
            QString forceKFpath = m_tempDir.path() + "/fkf_" + (isStart ? "start" : "end") + "."  + QFileInfo(m_videoPath).suffix();

            // Add keyframes at the specified timecode (m_startTime or m_endTime)
            QProcess *forceKeyFrame = processForceKeyframes(extractionPath1, {isStart ? (m_startTime - BT) : (m_endTime - BT)}, forceKFpath, m_codecParams);
            connect(forceKeyFrame, &QProcess::finished, [this, forceKeyFrame, isStart, extractionPath1, forceKFpath, BT, AT](){
                if (forceKeyFrame->exitStatus() != QProcess::NormalExit || forceKeyFrame->exitCode() != 0){
                    qDebug() << "[Sequence extraction] Error in force keyframes for " + QString(isStart ? "start" : "end");
                    qDebug() << "Exit Status : " << forceKeyFrame->exitStatus() << " exitCode : " << forceKeyFrame->exitCode() << "errors : " << forceKeyFrame->readAllStandardError();
                }
                QString extractionPath2 = m_tempDir.path() + "/extract2_" + (isStart ? "start" : "end") + "."  + QFileInfo(m_videoPath).suffix();

                // Second extraction, between specified timecode and first keyframe before/after
                int intervalStart = isStart ? m_startTime - BT : 0;
                int intervalEnd = isStart ? AT - BT : m_endTime - BT;
                qDebug() << "BT =" << BT;
                qDebug() << "AT =" << AT;
                qDebug() << "intervalStart =" << intervalStart;
                qDebug() << "intervalEnd =" << intervalEnd;
                QProcess *extract2 = extractSequence(forceKFpath, intervalStart, intervalEnd, extractionPath2, ExtractionType::Original, false);
                connect(extract2, &QProcess::finished, [this, extract2, isStart, extractionPath1, forceKFpath, BT, AT](){
                    if (extract2->exitStatus() != QProcess::NormalExit || extract2->exitCode() != 0)
                        qDebug() << "[Sequence extraction] Error in second extraction for " + QString(isStart ? "start" : "end");
                        qDebug() << "Exit Status : " << extract2->exitStatus() << " exitCode : " << extract2->exitCode() << "errors : " << extract2->readAllStandardError();
                    QFile(extractionPath1).remove();
                    QFile(forceKFpath).remove();
                    emit stepFinished();
                });
            });
        });
    }

    // inline void extractSequenceLossless(const QString& savePath)
    // {
    //     m_tempDir = QTemporaryDir();
    //     m_segmentList = new QFile(m_tempDir.path() + "/concat.txt");
    //     KeyframeBounds kb;
    //     findKeyframeBounds(m_videoPath, m_startTime, m_endTime, kb);
    //     m_codecParams = getVideoCodecParams(m_videoPath);

    //     QString startSegment = m_tempDir.path() + "/extract2_start." + QFileInfo(m_videoPath).suffix();
    //     QString copySegment = m_tempDir.path() + "/copy." + QFileInfo(m_videoPath).suffix();
    //     QString endSegment = m_tempDir.path() + "/extract2_end." + QFileInfo(m_videoPath).suffix();
    //     if ( m_segmentList->open(QIODevice::ReadWrite | QIODevice::Append) )
    //     {
    //         QTextStream stream(m_segmentList);
    //         stream << "file '" << startSegment << "'" << Qt::endl;
    //         stream << "file '" << copySegment << "'" << Qt::endl;
    //         stream << "file '" << endSegment << "'" << Qt::endl;
    //         m_segmentList->close();
    //     }
    //     connect(this, &SequenceExtractionHelper::stepFinished, [this, savePath](){
    //         finishSequenceExtraction(savePath);
    //     });
    //     reencodeTimecode(true, kb.BS, kb.AS);
    //     QProcess *copyProcess = extractSequence(m_videoPath, kb.AS, kb.BE, copySegment);
    //     connect(copyProcess, &QProcess::finished, [this, copyProcess](){
    //         if (copyProcess->exitStatus() != QProcess::NormalExit || copyProcess->exitCode() != 0)
    //             qDebug() << "[Sequence extraction] Error in copy";
    //         emit stepFinished();
    //     });
    //     reencodeTimecode(false, kb.BE, kb.AE);
    // }

    inline QProcess* reencodeExtractSequence(const QString& filePath, int startTime, int endTime, const QString& savePath, ExtractionType exportType = ExtractionType::Original)
    {
        if(exportType == ExtractionType::AudioOnly){
            return extractSequence(filePath, startTime, endTime, savePath, exportType);
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

        // m_codecParams = getVideoCodecParams(filePath);

        // on découpe une séquence grossière pour pouvoir y placer des keyframes sans encoder toute la vidéo
        QProcess *keyFrameCutProcess = extractSequence(filePath, startKeyFrameCut, endKeyFrameCut, savePathTemp, ExtractionType::Original, false);
        connect(keyFrameCutProcess, &QProcess::finished, [this, keyFrameCutProcess, startTime, endTime, cutStartTime, cutEndTime, savePath, savePathTemp, savePathTemp2](){
            qDebug() << "Exit Status : " << keyFrameCutProcess->exitStatus() << " exitCode : " << keyFrameCutProcess->exitCode() << "errors : " << keyFrameCutProcess->readAllStandardError();
            if (keyFrameCutProcess->exitStatus() == QProcess::NormalExit && keyFrameCutProcess->exitCode() == 0){
                QProcess *addKeyFramesProcess = processForceKeyframes(savePathTemp, {cutStartTime, cutEndTime}, savePathTemp2, m_codecParams);
                connect(addKeyFramesProcess, &QProcess::finished, [this, addKeyFramesProcess, startTime, endTime, cutStartTime, cutEndTime, savePath, savePathTemp, savePathTemp2](){
                    if (addKeyFramesProcess->exitStatus() == QProcess::NormalExit && addKeyFramesProcess->exitCode() == 0){
                        qDebug() << "start : " << cutStartTime << " end : " << cutEndTime;
                        QProcess *ffmpeg = extractSequence(savePathTemp2, cutStartTime, cutEndTime, savePath, ExtractionType::Original, false);
                        connect(ffmpeg, &QProcess::finished, [this, ffmpeg, savePathTemp, savePathTemp2](){
                            qDebug() << "Exit Status : " << ffmpeg->exitStatus() << " exitCode : " << ffmpeg->exitCode() << "errors : " << ffmpeg->readAllStandardError();
                            QFile(savePathTemp).remove();
                            QFile(savePathTemp2).remove();
                            emit this->extractionFinished(1);
                        });
                    }else{
                        qDebug() << "error when adding keyframes.";
                        qDebug() << "Exit Status : " << addKeyFramesProcess->exitStatus() << " exitCode : " << addKeyFramesProcess->exitCode() << "errors : " << addKeyFramesProcess->readAllStandardError();
                        emit this->extractionFinished(-3);
                    }
                });
            }
            else{
                qDebug() << "error when extracting for keyframes.";
                emit this->extractionFinished(-2);
            }
        });
        return nullptr;
    }

public slots:
    inline void finishSequenceExtraction(const QString& savePath)
    {
        m_progression++;
        if(m_progression != 3) return;

        m_progression = 0;
        qDebug() << QFileInfo(*m_segmentList).filePath();
        QProcess *concatProcess = concatenateSequences(QFileInfo(*m_segmentList).filePath(), savePath);
        connect(concatProcess, &QProcess::finished, [this, concatProcess](){
            if (concatProcess->exitStatus() != QProcess::NormalExit || concatProcess->exitCode() != 0){
                qDebug() << "[Sequence extraction] Error in concatenation";
                qDebug() << "Exit Status : " << concatProcess->exitStatus() << " exitCode : " << concatProcess->exitCode() << "errors : " << concatProcess->readAllStandardError();
            }
            m_tempDir.remove();
            emit extractionFinished(1);
        });

    }

signals:
    void extractionFinished(const int exitCode);
    void stepFinished();
    void progressStep(const int);
};

#endif // SEQUENCEEXTRACTIONHELPER_H
