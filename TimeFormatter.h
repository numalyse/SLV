#include <cstdint>

#include <QString>

namespace TimeFormatter
{

    
    inline QString msToHHMMSSFF(int64_t ms, float fps){
        int64_t timeInSeconds = ms / 1000;
        int64_t h = timeInSeconds / 3600;
        int64_t m = (timeInSeconds % 3600) / 60;
        int64_t s = timeInSeconds % 60;

        float tf = 1000.0f / fps;
        int frame = static_cast<int>((ms % 1000) / tf);

        return QString("%1:%2:%3:%4")
                .arg(h, 2, 10, QChar('0'))
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
                .arg(frame, 2, 10, QChar('0'));
    }
    


} // namespace TimeFormatter
