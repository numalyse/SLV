#include "FormShortcutEditFrame.h"

#include "PrefManager.h"
#include "GenericDialog.h"

#include <QLabel>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QStyleHints>
#include <QGuiApplication>

FormShortcutEditFrame::FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
: BasePreferenceFrame(name, subCategory, key, value, parent)
{
    m_keySequenceEdit = new QKeySequenceEdit(value, this);
    m_keySequenceEdit->setMaximumSequenceLength(1);
    m_keySequenceEdit->setToolTip(PrefManager::instance().getText("click_to_modify"));
    applyFrameStyleToChild(m_keySequenceEdit);

    m_keySequenceEdit->setFixedHeight(30);
    m_keySequenceEdit->setFixedWidth(150);

    QString backgroundFillColor;

#ifdef Q_OS_MAC
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        backgroundFillColor = "palette(mid)";
    } else {
        backgroundFillColor = "palette(base)";
    }
#else
    backgroundFillColor = "palette(base)";
#endif

    if (QLineEdit* internalLineEdit = m_keySequenceEdit->findChild<QLineEdit*>()) {
        internalLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        internalLineEdit->setStyleSheet(R"(
            QLineEdit{
                background-color : )" + backgroundFillColor + R"(;
                font : normal;
            }

            QLineEdit:focus{
                color : red;
                background-color : palette(Window);
                font : bold;
            }
        )");
    }

    setRightWidget(m_keySequenceEdit);

    connect(m_keySequenceEdit, &QKeySequenceEdit::editingFinished, this, [this](){
        if (!m_keySequenceEdit->hasFocus()) return;
        
        QString newShortcutString = m_keySequenceEdit->keySequence().toString();
        
        if (newShortcutString != m_prevValue) {
            this->setFocus();
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