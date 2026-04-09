#include "FormComboBoxFrame.h"

FormComboBoxFrame::FormComboBoxFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, const QStringList& values, QWidget *parent)
: BasePreferenceFrame(name, subCategory, key, value, parent)
{
    m_comboBox = new QComboBox(this);
    m_comboBox->addItems(values);

    m_comboBox->setCurrentText(value); 

    setRightWidget(m_comboBox);

    connect(m_comboBox, &QComboBox::currentTextChanged, this, [this](const QString& newText){
        if (newText != m_prevValue) {
            m_prevValue = newText;
            emit updateJsonObjRequested(m_subCategory, m_key, newText);
        }
    });

}


void FormComboBoxFrame::setUIValue(const QString& value){
    if(m_comboBox){
        m_comboBox->blockSignals(true);
        m_comboBox->setCurrentText(value); 
        m_comboBox->blockSignals(false);
    }
}