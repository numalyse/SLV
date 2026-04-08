#include "InterfaceTab.h"

#include "PrefManager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : QScrollArea(parent)
{
    auto& prefManager = PrefManager::instance();
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(this);
    QFormLayout* layout = new QFormLayout(container);


    m_baseInterface = prefManager.getCategory("Interface");
    m_updatedInterface = m_baseInterface;

    QComboBox* langComboBox = new QComboBox(this);
    QStringList availableLangs = prefManager.getAvailableLangs();
    for (size_t i = 0; i < availableLangs.size(); i++){
        langComboBox->addItem(availableLangs[i], i);
    }

    QString preferredLang = prefManager.getPref("Interface", "Lang", "code");
    int indexCurrLang = availableLangs.indexOf(preferredLang);
    if (indexCurrLang != -1) langComboBox->setCurrentIndex(indexCurrLang);

    connect(langComboBox, &QComboBox::currentIndexChanged, this , [this, availableLangs](int cbIndex){
        QJsonObject langObj = m_updatedInterface["Lang"].toObject();
        langObj["code"] = availableLangs[cbIndex];
        m_updatedInterface["Lang"] = langObj;
    });

    m_discardActions.append([this, langComboBox, availableLangs]() {
        QString originalLang = m_baseInterface["Lang"].toObject().value("code").toString();
        int index = availableLangs.indexOf(originalLang);
        if (index != -1) {
            langComboBox->blockSignals(true);
            langComboBox->setCurrentIndex(index);
            langComboBox->blockSignals(false);
        }
    });
    
    layout->addWidget(langComboBox);
    
    setWidget(container);
}

bool InterfaceTab::needSave()
{
    qDebug()<< " should save ? " << (m_baseInterface != m_updatedInterface);
    return m_baseInterface != m_updatedInterface;
}

void InterfaceTab::save()
{
    if(!needSave()) return;

    auto& prefManager = PrefManager::instance();
    if( prefManager.setCategory("Interface", m_updatedInterface)) {
        m_baseInterface = m_updatedInterface;
    } else {
        qWarning() << "[InterfaceTab] Echec de la sauvegarde des raccourcis";
    }
}

void InterfaceTab::discard()
{
    if(!needSave()) return;

    for (auto& function : m_discardActions)
    {
        function();
    }
    
    m_updatedInterface = m_baseInterface;

}