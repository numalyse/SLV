#include "Timeline/TimelineMath.h"
#include "TimelineMath.h"

TimelineMath::TimelineMath(double fps, int64_t duration, QObject *parent)
: QObject(parent), m_fps{fps}, m_duration{duration}
{
}

void TimelineMath::fitToWidth(double width)
{
    m_pixelsPerMs = width / static_cast<double>(m_duration);
}

int64_t TimelineMath::frameToTime(int frame)
{
    if (frame <= 0) return 0;
    if (m_fps <= 0.0) return 0; 

    double msPerFrame = 1000.0 / m_fps;
    double exactTimeMs = frame * msPerFrame;
    int64_t ms = static_cast<int64_t>( exactTimeMs );

    return std::min(ms, m_duration);
}

int64_t TimelineMath::posToTime(double pos)
{
    if(pos <= 0) return 0;
    double rawMs = pos / m_pixelsPerMs; 
    int64_t finalMs = static_cast<int64_t>(rawMs);
    return std::clamp(finalMs, static_cast<int64_t>(0), m_duration);
}

int64_t TimelineMath::posToTimeSnapped(double pos)
{
    double rawMs = pos / m_pixelsPerMs; 

    // snap à la frame la plus proche
    if (m_fps > 0) {
        double frameMs = 1000.0 / m_fps;
        double nearestFrameIndex = std::round(rawMs / frameMs);
        rawMs = nearestFrameIndex * frameMs; 
    }

    int64_t finalMs = static_cast<int64_t>(rawMs);
    return std::clamp(finalMs, static_cast<int64_t>(0), m_duration);
}

double TimelineMath::timeToPos(int64_t time)
{
    if(time <= 0) return 0;
    return static_cast<double>(time * m_pixelsPerMs); 
}
