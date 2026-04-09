#include "FormShortcutEditFrame.h"

#include "PrefManager.h"
#include "GenericDialog.h"

#include <QLabel>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QMessageBox>

FormShortcutEditFrame::FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
: BasePreferenceFrame(name, subCategory, key, value, parent)
{
    m_keySequenceEdit = new QKeySequenceEdit(value, this);
    m_keySequenceEdit->setMaximumSequenceLength(1);
    
    if (QLineEdit* internalLineEdit = m_keySequenceEdit->findChild<QLineEdit*>()) {
        internalLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    setRightWidget(m_keySequenceEdit);

    connect(m_keySequenceEdit, &QKeySequenceEdit::editingFinished, this, [this](){
        if (!m_keySequenceEdit->hasFocus()) return;
        
        QString newShortcutString = m_keySequenceEdit->keySequence().toString();
        
        if (newShortcutString != m_prevValue) {
            emit updateJsonObjRequested(m_subCategory, m_key, newShortcutString);
        }
    });
}

void FormShortcutEditFrame::clearShortcutUI(){
    if (m_keySequenceEdit) {
        m_keySequenceEdit->blockSignals(true); 
        m_prevValue = "";
        m_keySequenceEdit->clear();     
        m_keySequenceEdit->blockSignals(false);
    }
}

void FormShortcutEditFrame::setUIValue(const QString& shortcut){
    if (m_keySequenceEdit) {
        m_keySequenceEdit->blockSignals(true); 
        m_prevValue = shortcut;
        m_keySequenceEdit->setKeySequence(QKeySequence::fromString(shortcut));
        m_keySequenceEdit->blockSignals(false);
    }
}