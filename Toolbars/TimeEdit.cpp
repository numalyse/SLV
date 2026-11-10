#include "Toolbars/TimeEdit.h"
#include "TimeEdit.h"
#include "TimeValidator.h"
#include "PrefManager.h"
#include "TimeFormatter.h"

#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QStyleHints>
#include <algorithm>
#include <cmath>

TimeEdit::TimeEdit(const QString &txt, QWidget *parent, const bool hasAnimations) : QLineEdit(parent), m_hasAnimations(hasAnimations)
{
    setText(txt);
    setInputMask("99:99:99.99;0");
    m_timeValidator = new TimeValidator(this);
    setValidator(m_timeValidator);
    setFocusPolicy(Qt::ClickFocus);
    
    setFixedWidth(TimeEdit::s_widthMin);
    if (!m_hasAnimations) setFixedWidth(TimeEdit::s_widthMax);
    setAlignment(Qt::AlignCenter);

    QString userTheme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";

    m_paste = addAction(QIcon(":/icons/paste"+userTheme), QLineEdit::TrailingPosition);
    if (m_hasAnimations) m_paste->setVisible(false);
    m_paste->setToolTip(PrefManager::instance().getText("paste"));

    m_copy = addAction(QIcon(":/icons/copy"+userTheme), QLineEdit::TrailingPosition);
    if (m_hasAnimations) m_copy->setVisible(false);
    m_copy->setToolTip(PrefManager::instance().getText("copy"));

    m_action = addAction(QIcon(":/icons/check"+userTheme), QLineEdit::TrailingPosition);
    if (m_hasAnimations) m_action->setVisible(false);
    m_action->setToolTip(PrefManager::instance().getText("confirm"));

    connect(m_action, &QAction::triggered, this, [this](){
        emit returnPressed();
    });

    connect(m_copy, &QAction::triggered, this, [this](){
        QGuiApplication::clipboard()->setText(text());
        emit returnPressed();
    });

    connect(m_paste, &QAction::triggered, this, [this](){
        QString pastedText = QGuiApplication::clipboard()->text();
        QRegularExpression re("^\\d{2}:\\d{2}:\\d{2}\\.\\d{2}$");
        if(re.match(pastedText).hasMatch()){
            setText(pastedText);
            emit returnPressed();
        }
    });

}

void TimeEdit::setFrameRate(double fps)
{
    if (fps <= 0.0) {
        return;
    }

    m_fps = fps;
}

void TimeEdit::setMaximumTime(int64_t time)
{
    m_maxTime = std::max<int64_t>(0, time);
}

void TimeEdit::normalizeTimecode()
{
    if (m_normalizingTimecode) {
        return;
    }

    QStringList parts = text().split(QRegularExpression("[:.]"));
    if (parts.size() != 4) {
        return;
    }

    const int currentCursorPosition = cursorPosition();
    int values[4];
    for (int i = 0; i < 4; ++i) {
        bool isNumeric = false;
        values[i] = parts[i].replace(' ', '0').toInt(&isNumeric);
        if (!isNumeric) {
            values[i] = 0;
        }
    }

    const int framesPerSecond = std::max(1, static_cast<int>(std::lround(m_fps)));
    values[0] = std::clamp(values[0], 0, 99);
    values[1] = std::clamp(values[1], 0, 59);
    values[2] = std::clamp(values[2], 0, 59);
    values[3] = std::clamp(values[3], 0, framesPerSecond - 1);

    const QString timecode = QString("%1:%2:%3.%4")
                                 .arg(values[0], 2, 10, QChar('0'))
                                 .arg(values[1], 2, 10, QChar('0'))
                                 .arg(values[2], 2, 10, QChar('0'))
                                 .arg(values[3], 2, 10, QChar('0'));
    const int64_t time = std::min(TimeFormatter::HHMMSSFFToMs(timecode, m_fps), m_maxTime);
    m_normalizingTimecode = true;
    setText(TimeFormatter::msToHHMMSSFF(time, m_fps));
    setCursorPosition(std::min(currentCursorPosition, static_cast<int>(text().size())));
    m_normalizingTimecode = false;
}

void TimeEdit::keyPressEvent(QKeyEvent* event)
{
    const int oldCursorPosition = cursorPosition();
    const bool isDigit = event->text().size() == 1 && event->text().at(0).isDigit();
    QLineEdit::keyPressEvent(event);

    if (!isDigit || (oldCursorPosition != 1 && oldCursorPosition != 4 && oldCursorPosition != 7)) {
        return;
    }

    const int nextFieldPosition = oldCursorPosition == 1 ? 3 : oldCursorPosition == 4 ? 6 : 9;
    normalizeTimecode();
    setCursorPosition(nextFieldPosition);
}

void TimeEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn();
    if (!m_hasAnimations) return;
    setAlignment(Qt::AlignLeft);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(TimeEdit::s_widthMax);
    animMin->setStartValue(width());
    animMin->setEndValue(TimeEdit::s_widthMax);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(TimeEdit::s_widthMax);
    animMax->setStartValue(width());
    animMax->setEndValue(TimeEdit::s_widthMax);

    connect(animMax, &QPropertyAnimation::finished, this, [this](){
        if (this->hasFocus()) {
            m_action->setVisible(true);
            m_copy->setVisible(true);
            m_paste->setVisible(true);
        }
    });

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}

void TimeEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();
    if (!m_hasAnimations) return;
    setAlignment(Qt::AlignCenter);

    m_action->setVisible(false);
    m_copy->setVisible(false);
    m_paste->setVisible(false);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(TimeEdit::s_widthMax);
    animMin->setStartValue(width());
    animMin->setEndValue(TimeEdit::s_widthMin);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(TimeEdit::s_widthMax);
    animMax->setStartValue(width());
    animMax->setEndValue(TimeEdit::s_widthMin);

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}
