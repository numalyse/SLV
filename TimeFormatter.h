#include <cstdint>

#include <QString>

namespace TimeFormatter
{

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

        return QString("%1:%2:%3:%4")
                .arg(h, 2, 10, QChar('0'))
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
                .arg(frame, 2, 10, QChar('0'));
    }


} // namespace TimeFormatter
