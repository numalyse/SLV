#ifndef TIMELINEMATH_H
#define TIMELINEMATH_H

#include <QObject>

class TimelineMath : public QObject
{
Q_OBJECT

public:
    explicit TimelineMath(double fps, int64_t duration, QObject* parent = nullptr);
    
    double fps() { return m_fps;}
    int64_t duration() { return m_duration;}
    double pixelsPerMs() { return m_pixelsPerMs; }

    void setPixelsPerMs(double newPixelsPerMs) { m_pixelsPerMs = newPixelsPerMs;}
    void fitToWidth(double width);

    int64_t frameToTime(int frame);
    int64_t posToTime(double pos);
    int64_t posToTimeSnapped(double pos);
    double timeToPos(int64_t time);

private:
    
    double m_fps{};
    int64_t m_duration{};
    double m_pixelsPerMs {};


};




#endif