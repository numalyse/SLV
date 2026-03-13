#include "FormTextEdit.h"

#include "SignalManager.h"

FormTextEdit::FormTextEdit(const QString &text, bool editable, QWidget *parent)
{
    setText(text);
    setReadOnly(!editable);
    if(editable){
        connect(this, &FormTextEdit::focusIn, &SignalManager::instance(), &SignalManager::formLineEditPause );
        connect(this, &FormTextEdit::focusOut, &SignalManager::instance(), &SignalManager::formLineEditPlay );
    }
}

void FormTextEdit::focusInEvent(QFocusEvent *e)
{    
    QTextEdit::focusInEvent(e);
    emit focusIn();
}

void FormTextEdit::focusOutEvent(QFocusEvent *e)
{    
    QTextEdit::focusOutEvent(e);
    emit focusOut();
}
