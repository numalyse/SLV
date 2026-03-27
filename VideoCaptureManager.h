#ifndef VIDEOCAPTUREMANAGER_H
#define VIDEOCAPTUREMANAGER_H

#include "SequenceExtractionHelper.h"
#include <vlc/vlc.h>
#include <QString>
#include <QDir>

/// @brief Class used to record a media (Video and Audio) in a single MediaWidget. Supports time warps with mediaCutAndConcat method.
class VideoCaptureManager : QObject
{

public:

    VideoCaptureManager() {};
    /// @param filePath : path of the media
    void setMediaPath(const QString& filePath);
    /// @brief Initialize a temporary directory in which media segments will be stored.
    void initMediaTempDirectory();
    /// @brief Delete the temporary directory in which media segments are stored
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

    inline static const QString m_tempDirectoryPath = QDir::homePath() + "/SLV_Content/concat_videos_temp";
    QString m_concatMediaPath;
    QFile* m_concatFile = nullptr;
    QFileInfo m_mediaFile;
    unsigned int m_dirIndex = 0;
    int m_concatRecordNumber = 0;
    int m_startRecordTime = -1;
};

#endif // VIDEOCAPTUREMANAGER_H
