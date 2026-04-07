#include "InterfaceTab.h"

#include "PrefManager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : QWidget(parent)
{
    auto& prefManager = PrefManager::instance();
    QFormLayout* layout = new QFormLayout(this);

    QComboBox* langComboBox = new QComboBox(this);

    QStringList availableLangs = prefManager.getAvailableLangs();
    for (size_t i = 0; i < availableLangs.size(); i++)
    {
        langComboBox->addItem(availableLangs[i], i);
    }

    QString preferredLang = prefManager.getPref("Lang", "code");
    int indexCurrLang = availableLangs.indexOf(preferredLang);
    if (indexCurrLang != -1) langComboBox->setCurrentIndex(indexCurrLang);

    connect(langComboBox, &QComboBox::currentIndexChanged, this , [this, availableLangs](int cbIndex){
        auto& prefManager = PrefManager::instance();
        prefManager.setPref("Lang", "code", availableLangs[cbIndex]);
        QMessageBox msgBox(this); 
        msgBox.setWindowTitle(prefManager.getText("dialog_lang_updated_tittle"));
        msgBox.setText(prefManager.getText("dialog_lang_updated_text"));
        msgBox.setInformativeText(prefManager.getText("dialog_lang_updated_restart_text"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    });
    
    layout->addWidget(langComboBox);

}