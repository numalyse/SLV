#ifndef TIMEFORMATTER_H
#define TIMEFORMATTER_H

#include <cstdint>
#include <cmath>

#include <QString>
#include <QList>
#include <QDebug>

namespace TimeFormatter
{

    /// @brief Splits a ms time into (h, m, s, frame) as a "frame-based" non-drop timecode.
    /// now we use the frame number as base and not ms
    inline void msToTimecodeParts(int64_t ms, double fps,
                                  int64_t& h, int64_t& m, int64_t& s, int64_t& frame)
    {
        if (fps <= 0.0) fps = 1.0;
        if (ms < 0) ms = 0;

        double frameMs = 1000.0 / fps;
        int64_t totalFrames = static_cast<int64_t>(std::llround(ms / frameMs));

        int fpsi = static_cast<int>(std::lround(fps)); // timecode frames per second
        if (fpsi < 1) fpsi = 1;

        frame = totalFrames % fpsi;
        int64_t totalSeconds = totalFrames / fpsi;
        h = totalSeconds / 3600;
        m = (totalSeconds % 3600) / 60;
        s = totalSeconds % 60;
    }

    /// @brief Convertie str en int64_t
    /// @param timeStr 
    /// @param fps 
    /// @param frameOffset Si vlc set time nous place sur la frame précédente, on peut ajouter un offset frame pour compenser
    /// @return 
    inline int64_t HHMMSSFFToMs(QString timeStr, double fps, double frameOffset = 0.0) {
        timeStr.replace(' ', '0');
        timeStr.replace('.', ':');
        QStringList parts = timeStr.split(':');

        if (parts.size() < 4) {
            qDebug() << "Conversion impossible, le texte n'est pas au bon format";
            return 0;
        }

        if (fps <= 0.0) {
            qDebug() << "Conversion Warning, les fps étaient invalides, passage à 1 fps";
            fps = 1.0;
        }

        int64_t h = parts[0].toLongLong();
        int64_t m = parts[1].toLongLong();
        int64_t s = parts[2].toLongLong();

        int fpsi = static_cast<int>(std::lround(fps)); // timecode frames per second
        if (fpsi < 1) fpsi = 1;

        // frame-based non-drop timecode: rebuild the global frame index then the ms.
        // frameOffset stays expressed in frames (e.g. 0.05) to compensate an imprecise VLC seek.
        double totalFrames = static_cast<double>((h * 3600 + m * 60 + s) * fpsi)
                             + parts[3].toDouble() + frameOffset;

        double frameMs = 1000.0 / fps;
        return static_cast<int64_t>(std::llround(totalFrames * frameMs));
    }

    /// @brief Formatte un temps passé en ms en temps HH : MM : SS : FF en fonction du nombre de FPS du média
    /// @param ms 
    /// @param fps
    /// @return 
    inline QString msToHHMMSSFF(int64_t ms, double fps){
        int64_t h, m, s, frame;
        msToTimecodeParts(ms, fps, h, m, s, frame);

        return QString("%1:%2:%3.%4")
                .arg(h, 2, 10, QChar('0'))
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
                .arg(frame, 2, 10, QChar('0'));
    }

    inline QString fileFormatMsToHHMMSSFF(int64_t ms, double fps){
        int64_t h, m, s, frame;
        msToTimecodeParts(ms, fps, h, m, s, frame);

        return QString("%1-%2-%3-%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(frame, 2, 10, QChar('0'));
    }

    /// @brief Formats a duration in ms as HH:MM:SS, seconds truncated, no frame field.
    /// For durations (playlist total, media length) where a frame index is not needed
    inline QString msToHHMMSS(int64_t ms){
        if (ms < 0) ms = 0;
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;

        return QString("%1:%2:%3")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'));
    }

    inline QString msToHHMMSSMilMil(int64_t ms){
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;
        int64_t mil = ms % 1000;

        return QString("%1:%2:%3.%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(mil, 3, 10, QChar('0'));
    }

    inline QList<int64_t> parsedMsToHHMMSSFF(int64_t ms, double fps){
        QList<int64_t> res = {0,0,0,0};
        msToTimecodeParts(ms, fps, res[0], res[1], res[2], res[3]);
        return res;
    }


} // namespace TimeFormatter

#endif // TIMEFORMATTER_H
