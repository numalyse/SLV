#ifndef VIDEOCAPTUREHELPER_H
#define VIDEOCAPTUREHELPER_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QDebug>

namespace SLV {

/// @brief Opens a media file in an OpenCV VideoCapture, trying several backends by using cv::CAP_ANY.
/// @param cap : capture to open (reused as-is by OpenCV on each attempt)
/// @param mediaPath : path of the media to open
/// @param context : label prefixed to the warning logs (e.g. "Thumbnail")
/// @return true if one of the backends managed to open the media
inline bool openVideoCapture(cv::VideoCapture& cap, const QString& mediaPath, const QString& context = "VideoCapture")
{
    const std::string pathUtf8 = mediaPath.toUtf8().constData();

    if (cap.open(pathUtf8, cv::CAP_ANY)) {
        qDebug() << context << ": opencv backend =" << QString::fromStdString(cap.getBackendName());
        return true;
    }

    qWarning() << context << ": impossible d'ouvrir le média " << mediaPath << ", aucun backend OpenCV trouvé" ;
    return false;
}

} // namespace SLV

#endif // VIDEOCAPTUREHELPER_H
