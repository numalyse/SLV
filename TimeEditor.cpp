#include "TimeEditor.h"

TimeEditor::TimeEditor(QWidget *parent, const int currentTime, const int maxTime, const int minTime, const int sliderMax, const double fps)
    : QWidget{parent}, milPerFrame(int(1000/fps))
{
    m_time = currentTime;
    m_fps = fps;
    m_minTime = minTime;
    m_maxTime = maxTime;
    createButtons(sliderMax);
    initUiLayout();
    updateLineEdits();
}

void TimeEditor::createButtons(const int sliderMax)
{
    m_hoursPlus = new QPushButton("+");
    m_hoursMinus = new QPushButton("-");
    m_minutesPlus = new QPushButton("+");
    m_minutesMinus = new QPushButton("-");
    m_secondsPlus = new QPushButton("+");
    m_secondsMinus = new QPushButton("-");
    m_framesPlus = new QPushButton("+");
    m_framesMinus = new QPushButton("-");
    m_hoursEdit = new QLineEdit("00");
    m_minutesEdit = new QLineEdit("00");
    m_secondsEdit = new QLineEdit("00");
    m_framesEdit = new QLineEdit("00");
    m_timeSlider = new QSlider(Qt::Horizontal);
    m_timeSlider->setRange(0, sliderMax);
    m_timeSlider->setValue(m_time);

    connect(m_hoursPlus, &QPushButton::released, this, [this](){ onTimeChanged(3600000); });
    connect(m_hoursMinus, &QPushButton::released, this, [this](){ onTimeChanged(-3600000); });
    connect(m_minutesPlus, &QPushButton::released, this, [this](){ onTimeChanged(60000); });
    connect(m_minutesMinus, &QPushButton::released, this, [this](){ onTimeChanged(-60000); });
    connect(m_secondsPlus, &QPushButton::released, this, [this](){ onTimeChanged(1000); });
    connect(m_secondsMinus, &QPushButton::released, this, [this](){ onTimeChanged(-1000); });
    connect(m_framesPlus, &QPushButton::released, this, [this](){ onTimeChanged(milPerFrame); });
    connect(m_framesMinus, &QPushButton::released, this, [this](){ onTimeChanged(-milPerFrame); });
    connect(m_timeSlider, &QSlider::valueChanged, this, [this](int value){ onTimeChanged(value - m_time); });
}

void TimeEditor::initUiLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *timeChangeLayout = new QHBoxLayout();

    QVBoxLayout *hourButtonsLayout = new QVBoxLayout();
    hourButtonsLayout->addWidget(m_hoursPlus);
    hourButtonsLayout->addWidget(m_hoursEdit);
    hourButtonsLayout->addWidget(m_hoursMinus);
    QVBoxLayout *minutesButtonsLayout = new QVBoxLayout();
    minutesButtonsLayout->addWidget(m_minutesPlus);
    minutesButtonsLayout->addWidget(m_minutesEdit);
    minutesButtonsLayout->addWidget(m_minutesMinus);
    QVBoxLayout *secondsButtonsLayout = new QVBoxLayout();
    secondsButtonsLayout->addWidget(m_secondsPlus);
    secondsButtonsLayout->addWidget(m_secondsEdit);
    secondsButtonsLayout->addWidget(m_secondsMinus);
    QVBoxLayout *frameButtonsLayout = new QVBoxLayout();
    frameButtonsLayout->addWidget(m_framesPlus);
    frameButtonsLayout->addWidget(m_framesEdit);
    frameButtonsLayout->addWidget(m_framesMinus);

    timeChangeLayout->addLayout(hourButtonsLayout);
    timeChangeLayout->addLayout(minutesButtonsLayout);
    timeChangeLayout->addLayout(secondsButtonsLayout);
    timeChangeLayout->addLayout(frameButtonsLayout);

    mainLayout->addLayout(timeChangeLayout);
    mainLayout->addWidget(m_timeSlider);

}

void TimeEditor::updateLineEdits()
{
    m_hoursEdit->blockSignals(true);
    m_minutesEdit->blockSignals(true);
    m_secondsEdit->blockSignals(true);
    m_framesEdit->blockSignals(true);
    m_timeSlider->blockSignals(true);

    QList<int64_t> parsedTimeCode = TimeFormatter::parsedMsToHHMMSSFF(m_time, m_fps);
    m_hours = parsedTimeCode[0];
    m_minutes = parsedTimeCode[1];
    m_seconds = parsedTimeCode[2];
    m_frames = parsedTimeCode[3];
    m_hoursEdit->setText(QString::number(m_hours));
    m_minutesEdit->setText(QString::number(m_minutes));
    m_secondsEdit->setText(QString::number(m_seconds));
    m_framesEdit->setText(QString::number(m_frames));
    m_timeSlider->setValue(m_time);

    m_hoursEdit->blockSignals(false);
    m_minutesEdit->blockSignals(false);
    m_secondsEdit->blockSignals(false);
    m_framesEdit->blockSignals(false);
    m_timeSlider->blockSignals(false);
}

void TimeEditor::onTimeChanged(const int amount)
{
    const int precedentTime = m_time;
    m_time += amount;
    m_time = std::min(m_maxTime, std::max(m_time, m_minTime));
    updateLineEdits();
    if(precedentTime != m_time)
        emit timeChanged(m_time+milPerFrame);
}

void TimeEditor::onMinTimeChanged(const int newMin)
{
    m_minTime = newMin;
    onTimeChanged(0);
}

// void TimeEditor::onHoursPlus()
// {
//     time +=
// }
