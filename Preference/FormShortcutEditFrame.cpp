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
        auto& prefManager = PrefManager::instance();
        auto [conflictSubCategory, conflictKey] = prefManager.checkAvailableShortcut(m_key, newShortcutString);

        if(conflictKey.isEmpty()) {
            m_prevValue = newShortcutString;
            emit updateJsonObjRequested(m_subCategory, m_key, newShortcutString);
        }
        else {
            SLV::showGenericDialog(
                nullptr, 
                prefManager.getText("dialog_update_shortcut_conflict_title"),
                prefManager.getText("dialog_update_shortcut_conflict_text") 
                + prefManager.getText("shortcut_subsection_" + conflictSubCategory) 
                + ", " + prefManager.getText("shortcut_" + conflictKey)
                + " : " + newShortcutString,

                prefManager.getText("dialog_update_shortcut_conflict_text_info"),
                
                [this, conflictSubCategory, conflictKey, newShortcutString]() { // si y'a un conflit, on assigne le raccourcis et on vide l'autre
                    auto& prefManager = PrefManager::instance();
                    m_prevValue = newShortcutString;
                    emit updateJsonObjRequested(m_subCategory, m_key, newShortcutString);
                    emit updateJsonObjRequested(conflictSubCategory, conflictKey, "");
                    emit emptyShortcutUIRequested(conflictKey);
                },

                nullptr,

                [this](){
                    auto& prefManager = PrefManager::instance();
                    m_keySequenceEdit->setKeySequence(QKeySequence::fromString(m_prevValue));
                }
            );
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