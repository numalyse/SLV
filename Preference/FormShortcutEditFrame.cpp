#include "FormShortcutEditFrame.h"

#include "PrefManager.h"

#include <QLabel>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QLineEdit>

FormShortcutEditFrame::FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
{
    setAutoFillBackground(true);
    setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");

    QFormLayout* layout = new QFormLayout(this);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setAlignment(Qt::AlignLeft);
    nameLabel->setStyleSheet("font-weight: bold;");
    nameLabel->setText(name);

    QKeySequenceEdit* keySequenceEdit = new QKeySequenceEdit(value, this);
    QLineEdit* internalLineEdit = keySequenceEdit->findChild<QLineEdit*>();  // récupère le QLineEdit interne généré par Qt
    if (internalLineEdit) {
        internalLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    connect(keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, this, [this, subCategory, key](const QKeySequence &keySequence){
        QString shortcutString = keySequence.toString();
        
        auto& prefManager = PrefManager::instance();
        prefManager.setPref("Shortcuts", subCategory, key, shortcutString);
    });

    
    layout->addRow(nameLabel, keySequenceEdit);
}

