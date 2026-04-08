#include "FormShortcutEditFrame.h"

#include "PrefManager.h"
#include "GenericDialog.h"

#include <QLabel>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QMessageBox>

FormShortcutEditFrame::FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
{
    setAutoFillBackground(true);
    setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");

    QFormLayout* layout = new QFormLayout(this);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setAlignment(Qt::AlignLeft);
    nameLabel->setStyleSheet("font-weight: bold;");
    nameLabel->setText(name);

    m_prevValue = value;
    m_keySequenceEdit = new QKeySequenceEdit(value, this);
    m_keySequenceEdit->setMaximumSequenceLength(1);
    QLineEdit* internalLineEdit = m_keySequenceEdit->findChild<QLineEdit*>();  // récupère le QLineEdit interne généré par Qt
    if (internalLineEdit) {
        internalLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    connect(m_keySequenceEdit, &QKeySequenceEdit::editingFinished, this, [this, subCategory, key](){
        if (!m_keySequenceEdit->hasFocus()) return;
        
        QString newShortcutString = m_keySequenceEdit->keySequence().toString();
        auto& prefManager = PrefManager::instance();

        auto [conflictSubCategory, conflictKey] = prefManager.checkAvailableShortcut(key, newShortcutString);

        if(conflictKey.isEmpty()) {
            m_prevValue = newShortcutString;
            emit updateJsonObjRequested(subCategory, key, newShortcutString);
        } 
        else {
            SLV::showGenericDialog(
                this, 
                prefManager.getText("dialog_update_shortcut_conflict_title"),
                prefManager.getText("dialog_update_shortcut_conflict_text"),
                
                [this, conflictSubCategory, conflictKey, subCategory, key, newShortcutString]() { // si y'a un conflit, on assigne le raccourcis et on vide l'autre
                    auto& prefManager = PrefManager::instance();
                    m_prevValue = newShortcutString;
                    emit updateJsonObjRequested(subCategory, key, newShortcutString);
                    emit updateJsonObjRequested(conflictSubCategory, conflictKey, "");
                    emit emptyShortcutUIRequested(conflictKey);
                },

                nullptr,

                [this, subCategory, key](){
                    auto& prefManager = PrefManager::instance();
                    m_keySequenceEdit->setKeySequence(QKeySequence::fromString(m_prevValue));
                }
            );
        }

    });

    
    layout->addRow(nameLabel, m_keySequenceEdit);
}

void FormShortcutEditFrame::clearShortcutUI(){
    if (m_keySequenceEdit) {
        m_keySequenceEdit->blockSignals(true); 
        m_prevValue = "";
        m_keySequenceEdit->clear();     
        m_keySequenceEdit->blockSignals(false);
    }
}

void FormShortcutEditFrame::setShortcutUI(const QString& shortcut){
    if (m_keySequenceEdit) {
        m_keySequenceEdit->blockSignals(true); 
        m_prevValue = shortcut;
        m_keySequenceEdit->setKeySequence(QKeySequence::fromString(shortcut));
        m_keySequenceEdit->blockSignals(false);
    }
}