#include "FormComboBoxFrame.h"
#include "PrefManager.h"

FormComboBoxFrame::FormComboBoxFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, const QStringList& values, QWidget *parent)
: BasePreferenceFrame(name, subCategory, key, value, parent)
{
    m_comboBox = new QComboBox(this);
    //m_comboBox->addItems(values);

    for (const QString &code : values)
    {
        m_comboBox->addItem(
            PrefManager::instance().getText(code), 
            code                                    
        );
    }

    int index = m_comboBox->findData(value);
    if (index != -1)
    {
        m_comboBox->setCurrentIndex(index);
    }

    m_comboBox->model()->sort(0);
    setRightWidget(m_comboBox);

    connect(m_comboBox,
            &QComboBox::currentIndexChanged,
            this,
            [this](int)
    {
        QString newValue = m_comboBox->currentData().toString();

        if (newValue != m_prevValue)
        {
            m_prevValue = newValue;
            emit updateJsonObjRequested(m_subCategory, m_key, newValue);
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