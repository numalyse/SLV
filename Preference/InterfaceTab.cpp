#include "InterfaceTab.h"

#include "PrefManager.h"
#include "Preference/BasePreferenceFrame.h"
#include "Preference/FormComboBoxFrame.h"


#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : BasePreferenceTab("Interface", parent) 
{
    auto& prefManager = PrefManager::instance();

    QString preferredLang = prefManager.getPref("Interface", "Lang", "code");
    QStringList availableLangs = prefManager.getAvailableLangs();

    FormComboBoxFrame* langFrame = new FormComboBoxFrame(
        prefManager.getText("interface_lang_code"), 
        "Lang",
        "code",    
        preferredLang, 
        availableLangs, 
        m_container
    ); 

    addPreferenceFrame(langFrame);

    m_layout->addRow(langFrame);
 
}
