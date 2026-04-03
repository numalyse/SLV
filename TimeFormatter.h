#ifndef TIMEFORMATTER_H
#define TIMEFORMATTER_H

#include <cstdint>

#include <QString>
#include <QList>
#include <QDebug>

namespace TimeFormatter
{

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

        int64_t h = parts[0].toLongLong() * 3600000;
        int64_t m = parts[1].toLongLong() * 60000;
        int64_t s = parts[2].toLongLong() * 1000;

        double frames = parts[3].toDouble() + frameOffset; 
        int64_t msFromFrames = static_cast<int64_t>( (frames * 1000.0) / fps);

        return h + m + s + msFromFrames;
    }

    /// @brief Formatte un temps passé en ms en temps HH : MM : SS : FF en fonction du nombre de FPS du média
    /// @param ms 
    /// @param fps
    /// @return 
    inline QString msToHHMMSSFF(int64_t ms, double fps){
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;

        double tf = 1000.0 / fps;
        int frame = static_cast<int>((ms % 1000) / tf);

        return QString("%1:%2:%3.%4")
                .arg(h, 2, 10, QChar('0'))
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
                .arg(frame, 2, 10, QChar('0'));
    }

    inline QString fileFormatMsToHHMMSSFF(int64_t ms, double fps){
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;

        double tf = 1000.0 / fps;
        int frame = static_cast<int>((ms % 1000) / tf);

        return QString("%1-%2-%3-%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(frame, 2, 10, QChar('0'));
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
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;

        double tf = 1000.0 / fps;
        int frame = static_cast<int>((ms % 1000) / tf);

        res[0] = h;
        res[1] = m;
        res[2] = s;
        res[3] = frame;
        return res;
    }


} // namespace TimeFormatter

#endif // TIMEFORMATTER_H
