#include "FormTextEdit.h"

#include "SignalManager.h"
#include "ProjectManager.h"

FormTextEdit::FormTextEdit(const QString &text, bool editable, QWidget *parent) : QTextEdit(parent)
{
    setText(text);
    setReadOnly(!editable);
    if(editable){
        connect(this, &FormTextEdit::focusIn, &SignalManager::instance(), &SignalManager::formLineEditPause );
        connect(this, &FormTextEdit::focusOut, &SignalManager::instance(), &SignalManager::formLineEditPlay );
        connect(this, &QTextEdit::textChanged, this, [this]() {
            if (this->hasFocus()) { // detecter que quand l'utilisateur met à jour le champ de texte
                ProjectManager::instance().setSaveNeeded();
            }
        });
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
