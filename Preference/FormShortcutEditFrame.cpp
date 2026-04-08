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

            prefManager.setPref("Shortcuts", subCategory, key, newShortcutString);
        } 
        else {
            SLV::showGenericDialog(
                this, 
                prefManager.getText("dialog_update_shortcut_conflict_title"),
                prefManager.getText("dialog_update_shortcut_conflict_text"),
                
                [this, conflictSubCategory, conflictKey, subCategory, key, newShortcutString]() { // si y'a un conflit, on assigne le raccourcis et on vide l'autre
                    auto& prefManager = PrefManager::instance();
                    prefManager.setPref("Shortcuts", subCategory, key, newShortcutString);
                    prefManager.setPref("Shortcuts", conflictSubCategory, conflictKey, ""); 
                    emit emptyShortcutOf(conflictKey);
                },

                nullptr,

                [this, subCategory, key](){
                    auto& prefManager = PrefManager::instance();
                    m_keySequenceEdit->setKeySequence(QKeySequence::fromString(prefManager.getPref("Shortcuts", subCategory, key)));
                }
            );
        }

    });

    
    layout->addRow(nameLabel, m_keySequenceEdit);
}

void FormShortcutEditFrame::clearShortcutUI(){
    QKeySequenceEdit* keyEdit = this->findChild<QKeySequenceEdit*>();
    if (keyEdit) {
        keyEdit->blockSignals(true); // On bloque pour ne pas relancer le check de conflit
        keyEdit->clear();            // Vide l'affichage du raccourci
        keyEdit->blockSignals(false);
    }
}