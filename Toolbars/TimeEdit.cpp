#include "Toolbars/TimeEdit.h"
#include "TimeEdit.h"
#include "TimeValidator.h"

#include <QPropertyAnimation>

TimeEdit::TimeEdit(const QString &txt, QWidget *parent) : QLineEdit(parent)
{
    setText(txt);
    setInputMask("99:99:99.99");
    setValidator(new TimeValidator(this));
    
    setFixedWidth(75);

    m_action = addAction(QIcon(":/icons/check"), QLineEdit::TrailingPosition);
    m_action->setVisible(false);

    connect(m_action, &QAction::triggered, this, [this](){
        emit returnPressed();
    });

}

void TimeEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn(); 

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(150);
    animMin->setStartValue(width());
    animMin->setEndValue(100);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(150);
    animMax->setStartValue(width());
    animMax->setEndValue(100);

    connect(animMax, &QPropertyAnimation::finished, this, [this](){
        if (this->hasFocus()) {
            m_action->setVisible(true);
        }
    });

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}

void TimeEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();

    m_action->setVisible(false);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(150);
    animMin->setStartValue(width());
    animMin->setEndValue(75);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(150);
    animMin->setStartValue(width());
    animMax->setEndValue(75);

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}
