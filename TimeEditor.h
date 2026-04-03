#ifndef TIMEEDITOR_H
#define TIMEEDITOR_H

#include "TimeFormatter.h"

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider.h>
#include <QLayout>

class TimeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TimeEditor(QWidget *parent = nullptr, const int currentTime = 0, const int maxTime = 10000000, const int minTime = 0, const int sliderMax = 10000000, const double fps = 24);

    void createButtons(const int sliderMax);
    void initUiLayout();
    void onHoursPlus();
    void onHoursMinus();
    void onMinutesPlus();
    void onMinutesMinus();
    void onSecondsPlus();
    void onSecondsMinus();
    void onFramePlus();
    void onFrameMinus();
    void onLineEdited();
    void onTimeChanged(const int amount);
    void updateLineEdits();

public slots:
    void onMinTimeChanged(const int newMin);

private:

    int m_time;
    int m_minTime;
    int m_maxTime;
    int m_hours;
    int m_minutes;
    int m_seconds;
    int m_frames;
    double m_fps;
    const int milPerFrame;

    QPushButton* m_hoursPlus;
    QPushButton* m_hoursMinus;
    QPushButton* m_minutesPlus;
    QPushButton* m_minutesMinus;
    QPushButton* m_secondsPlus;
    QPushButton* m_secondsMinus;
    QPushButton* m_framesPlus;
    QPushButton* m_framesMinus;
    QLineEdit* m_hoursEdit;
    QLineEdit* m_minutesEdit;
    QLineEdit* m_secondsEdit;
    QLineEdit* m_framesEdit;
    QSlider* m_timeSlider;

signals:
    void timeChanged(const int time);
};

#endif // TIMEEDITOR_H
