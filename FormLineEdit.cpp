#include "FormLineEdit.h"

#include "SignalManager.h"

FormLineEdit::FormLineEdit(const QString &text, bool editable, QWidget *parent) : QLineEdit{parent}
{
    setText(text);
    setReadOnly(!editable);
    if(editable){
        connect(this, &FormLineEdit::focusIn, &SignalManager::instance(), &SignalManager::formLineEditPause );
        connect(this, &FormLineEdit::focusOut, &SignalManager::instance(), &SignalManager::formLineEditPlay );
    }
}

void FormLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn();
}

void FormLineEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();

}
