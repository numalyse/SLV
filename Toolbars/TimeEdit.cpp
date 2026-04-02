#include "Toolbars/TimeEdit.h"
#include "TimeEdit.h"
#include "TimeValidator.h"

TimeEdit::TimeEdit(const QString &txt, QWidget *parent) : QLineEdit(parent)
{
    setText(txt);
    setInputMask("99:99:99.99");
    setValidator(new TimeValidator(this));
}

void TimeEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn();
}

void TimeEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();
}
