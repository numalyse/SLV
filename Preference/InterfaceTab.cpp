#include "InterfaceTab.h"

#include "PrefManager.h"
#include "Preference/BasePreferenceFrame.h"
#include "Preference/FormPathEditFrame.h"


#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : BasePreferenceTab("Interface", parent) 
{
    auto& prefManager = PrefManager::instance();

    QString preferredLang = prefManager.getPref("Interface", "Lang", "code");
    QStringList availableLangs = prefManager.getAvailableLangs();

/*     
    FormComboBoxFrame* langFrame = new FormComboBoxFrame(
        prefManager.getText("interface_lang_code"), 
        "Lang",      // Sous-catégorie
        "code",      // Clé
        availableLangs, 
        preferredLang, 
        m_container
    ); 
    
    // Ajout et connexion automatique
    addPreferenceFrame(langFrame);

    layout->addRow( prefManager.getText("interface_lang_code"), FormComboBoxFrame);
 */   

    FormPathEditFrame* projectPathFrame = new FormPathEditFrame(
        "Dossier de sauvegarde",          
        "Paths",                      
        "screenshot",           
        prefManager.getPref("Paths", "screenshot"),
        true,                           
        this
    );

    addPreferenceFrame( projectPathFrame );

}
