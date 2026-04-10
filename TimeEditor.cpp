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
    m_timeEdits = {m_hoursEdit, m_minutesEdit, m_secondsEdit, m_framesEdit};
    m_indexTimeEdit = -1;

    connect(m_hoursPlus, &QPushButton::released, this, [this](){ onTimeChanged(3600000); });
    connect(m_hoursMinus, &QPushButton::released, this, [this](){ onTimeChanged(-3600000); });
    connect(m_minutesPlus, &QPushButton::released, this, [this](){ onTimeChanged(60000); });
    connect(m_minutesMinus, &QPushButton::released, this, [this](){ onTimeChanged(-60000); });
    connect(m_secondsPlus, &QPushButton::released, this, [this](){ onTimeChanged(1000); });
    connect(m_secondsMinus, &QPushButton::released, this, [this](){ onTimeChanged(-1000); });
    connect(m_framesPlus, &QPushButton::released, this, [this](){ onTimeChanged(milPerFrame); });
    connect(m_framesMinus, &QPushButton::released, this, [this](){ onTimeChanged(-milPerFrame); });
    connect(m_timeSlider, &QSlider::valueChanged, this, [this](int value){ onTimeChanged(value - m_time); });
    connect(m_hoursEdit, &QLineEdit::textChanged, this, [this](const QString& str){
        if(m_hoursEdit->hasFocus()){
            onTimeEditsChanged(0);
            if(m_hoursEdit->cursorPosition() >= 2){
                m_minutesEdit->setFocus();
                m_minutesEdit->setCursorPosition(0);
            }
        }

    });
    connect(m_minutesEdit, &QLineEdit::textChanged, this, [this](const QString& str){
        if(m_minutesEdit->hasFocus()){
            onTimeEditsChanged(1);
            if(m_minutesEdit->cursorPosition() >= 2){
                m_secondsEdit->setFocus();
                m_secondsEdit->setCursorPosition(0);
            }
        }
    });
    connect(m_secondsEdit, &QLineEdit::textChanged, this, [this](const QString& str){
        if(m_secondsEdit->hasFocus()){
            onTimeEditsChanged(2);
            qDebug() << "CURSOR POS : " << m_secondsEdit->cursorPosition();
            if(m_secondsEdit->cursorPosition() >= 2){
                m_framesEdit->setFocus();
                m_framesEdit->setCursorPosition(0);
            }
        }
    });
    connect(m_secondsEdit, &QLineEdit::inputRejected, this, [this](){
        qDebug() << "omg ?";
    });
    connect(m_framesEdit, &QLineEdit::textChanged, this, [this](const QString& str){
        if(m_framesEdit->hasFocus()){
            onTimeEditsChanged(3);
        }
    });
}

void TimeEditor::initUiLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *timeChangeLayout = new QHBoxLayout();
    timeChangeLayout->setSpacing(3);
    timeChangeLayout->addStretch();
    timeChangeLayout->setContentsMargins(0,0,0,0);

    m_hoursEdit->setFixedWidth(40);
    m_minutesEdit->setFixedWidth(40);
    m_secondsEdit->setFixedWidth(40);
    m_framesEdit->setFixedWidth(40);
    m_hoursPlus->setFixedSize(40, 30);
    m_hoursMinus->setFixedSize(40, 30);
    m_minutesPlus->setFixedSize(40, 30);
    m_minutesMinus->setFixedSize(40, 30);
    m_secondsPlus->setFixedSize(40, 30);
    m_secondsMinus->setFixedSize(40, 30);
    m_framesPlus->setFixedSize(40, 30);
    m_framesMinus->setFixedSize(40, 30);

    m_hoursPlus->setFocusPolicy(Qt::NoFocus);
    m_hoursMinus->setFocusPolicy(Qt::NoFocus);
    m_minutesPlus->setFocusPolicy(Qt::NoFocus);
    m_minutesMinus->setFocusPolicy(Qt::NoFocus);
    m_secondsPlus->setFocusPolicy(Qt::NoFocus);
    m_secondsMinus->setFocusPolicy(Qt::NoFocus);
    m_framesPlus->setFocusPolicy(Qt::NoFocus);
    m_framesMinus->setFocusPolicy(Qt::NoFocus);
    m_hoursEdit->setAlignment(Qt::AlignCenter);
    m_minutesEdit->setAlignment(Qt::AlignCenter);
    m_secondsEdit->setAlignment(Qt::AlignCenter);
    m_framesEdit->setAlignment(Qt::AlignCenter);
    setFocus();

    m_hoursEdit->setInputMask("99");
    m_hoursEdit->setValidator(new TimeValidator(this));
    m_minutesEdit->setInputMask("99");
    m_minutesEdit->setValidator(new TimeValidator(this));
    m_secondsEdit->setInputMask("99");
    m_secondsEdit->setValidator(new TimeValidator(this));
    m_hoursEdit->setInputMask("99");
    m_hoursEdit->setValidator(new TimeValidator(this));

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
    timeChangeLayout->addWidget(new QLabel(":"));
    timeChangeLayout->addLayout(minutesButtonsLayout);
    timeChangeLayout->addWidget(new QLabel(":"));
    timeChangeLayout->addLayout(secondsButtonsLayout);
    timeChangeLayout->addWidget(new QLabel("."));
    timeChangeLayout->addLayout(frameButtonsLayout);
    timeChangeLayout->addStretch();

    mainLayout->addLayout(timeChangeLayout);
    mainLayout->addWidget(m_timeSlider);

    setStyleSheet(
        "QPushButton, QLineEdit{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "   font-size: 17px;"
        "}"
        "QPushButton:hover, QLineEdit:hover, QLineEdit:focus{"
        "   background-color: palette(button);"
        "   border: 1px solid palette(button);"
        "   border-radius: 4px;"
        "   font-size: 17px;"
        "}"
    );

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
    m_hoursEdit->setText(QString("%1").arg(m_hours, 2, 10, QChar('0')));
    m_minutesEdit->setText(QString("%1").arg(m_minutes, 2, 10, QChar('0')));
    m_secondsEdit->setText(QString("%1").arg(m_seconds, 2, 10, QChar('0')));
    m_framesEdit->setText(QString("%1").arg(m_frames, 2, 10, QChar('0')));
    m_timeSlider->setValue(m_time);

    m_hoursEdit->blockSignals(false);
    m_minutesEdit->blockSignals(false);
    m_secondsEdit->blockSignals(false);
    m_framesEdit->blockSignals(false);
    m_timeSlider->blockSignals(false);
}

void TimeEditor::onTimeChanged(const int amount)
{
    const int previousTime = m_time;
    m_time += amount;
    m_time = std::min(m_maxTime, std::max(m_time, m_minTime));
    updateLineEdits();
    if(previousTime != m_time)
        emit timeChanged(m_time+milPerFrame);
}

void TimeEditor::onMinTimeChanged(const int newMin)
{
    m_minTime = newMin;
    onTimeChanged(0);
}

void TimeEditor::onTimeEditsChanged(const int timeEditIndex)
{
    m_hoursEdit->blockSignals(true);
    m_minutesEdit->blockSignals(true);
    m_secondsEdit->blockSignals(true);
    m_framesEdit->blockSignals(true);
    m_timeSlider->blockSignals(true);

    int pos = m_timeEdits[timeEditIndex]->cursorPosition();
    const int previousTime = m_time;

    if(m_hoursEdit->text().size() > 2) m_hoursEdit->setText(m_hoursEdit->text().first(2));
    if(m_minutesEdit->text().size() > 2) m_minutesEdit->setText(m_minutesEdit->text().first(2));
    if(m_secondsEdit->text().size() > 2) m_secondsEdit->setText(m_secondsEdit->text().first(2));
    if(m_framesEdit->text().size() > 2) m_framesEdit->setText(m_framesEdit->text().first(2));

    int hEditInt = m_hoursEdit->text().toLongLong();
    int mEditInt = m_minutesEdit->text().toLongLong();
    int sEditInt = m_secondsEdit->text().toLongLong();
    int fEditInt = m_framesEdit->text().toLongLong();

    hEditInt = std::clamp(hEditInt, 0, 99);
    mEditInt = std::clamp(mEditInt, 0, 59);
    sEditInt = std::clamp(sEditInt, 0, 59);
    fEditInt = std::clamp(fEditInt, 0, int(m_fps));

    int64_t h = hEditInt * 3600000;
    int64_t m = mEditInt * 60000;
    int64_t s = sEditInt * 1000;

    m_time = h + m + s + (fEditInt*milPerFrame);
    m_time = std::min(m_maxTime, std::max(m_time, m_minTime));
    updateLineEdits();

    if(previousTime != m_time)
        emit timeChanged(m_time+milPerFrame);
    m_timeEdits[timeEditIndex]->setCursorPosition(pos);

    m_hoursEdit->blockSignals(false);
    m_minutesEdit->blockSignals(false);
    m_secondsEdit->blockSignals(false);
    m_framesEdit->blockSignals(false);
    m_timeSlider->blockSignals(false);
}

// void TimeEditor::onHoursPlus()
// {
//     time +=
// }
