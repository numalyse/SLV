#ifndef VIDEOCAPTUREMANAGER_H
#define VIDEOCAPTUREMANAGER_H

#include "SequenceExtractionHelper.h"
#include <vlc/vlc.h>
#include <QString>
#include <QDir>
#include <QTemporaryDir>

/// @brief Class used to record a media (Video and Audio) in a single MediaWidget. Supports time warps with mediaCutAndConcat method.
class VideoCaptureManager : public QObject
{
Q_OBJECT

public:

    VideoCaptureManager() {};
    ~VideoCaptureManager() { deleteMediaTempDirectory(); }
    /// @param filePath : path of the media
    void setMediaPath(const QString& filePath);
    /// @brief Initialize a temporary directory in which media segments will be stored.
    /// Uses Qt's QTemporaryDir (under QDir::tempPath()), so the directory is unique
    /// and removed automatically when it is released or the manager is destroyed.
    void initMediaTempDirectory();
    /// @brief Release the temporary directory in which media segments are stored.
    /// Destroying the QTemporaryDir removes the directory and its content. Safe to
    /// call multiple times.
    void deleteMediaTempDirectory();
    /// @brief Starts recording the media from startTime
    /// @param startTime : time in ms corresponding to the begining of the record in the media
    void startMediaRecording(const int startTime);
    /// @brief Cuts the media recording at cutTime before warpping to newTime. The clip is saved in the temporary directory
    /// waiting to be concatenated with other segments (in endMediaRecording).
    /// @param cutTime : end time of the current segment of the record (time before warpping to another timecode)
    /// @param newTime : start time of the new segment of the record (time after warpping to another timecode)
    void mediaCutAndConcat(const int cutTime, const int newTime);
    /// @brief Ends media recording and concatenate all of the media record segments from temporary directory into one media file.
    /// The temporary directory is then deleted
    /// @param endTime : ending time of the recorded segment
    void endMediaRecording(const int endTime, const QString& savePath);
private:

    QTemporaryDir* m_tempDir = nullptr;
    QString m_concatMediaPath;
    QFile* m_concatFile = nullptr;
    QFileInfo m_mediaFile;
    int m_concatRecordNumber = 0;
    int m_startRecordTime = -1;

signals:
    void recordSegmentDone(const QString& segmentPath);
    void recordSegmentFailed();
};

#endif // VIDEOCAPTUREMANAGER_H
