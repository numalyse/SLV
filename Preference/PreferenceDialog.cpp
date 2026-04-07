#include "PreferenceDialog.h"

#include "PrefManager.h"
#include "Preference/InterfaceTab.h"
#include "Preference/ShortcutTab.h"

#include <QFormLayout>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>

PreferenceDialog::PreferenceDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    auto& prefManager = PrefManager::instance();
    QFormLayout* prefManagerLayout = new QFormLayout(this);
    m_tabWidget = new QTabWidget(this);

    InterfaceTab* interfaceTab = new InterfaceTab(m_tabWidget);
    m_tabWidget->addTab(interfaceTab, prefManager.getText("pref_dialog_tab_interface"));


    ShortcutTab* shortcutTab = new ShortcutTab(m_tabWidget);
    m_tabWidget->addTab(shortcutTab, prefManager.getText("pref_dialog_tab_shortcut"));

    prefManagerLayout->addWidget(m_tabWidget);
    
}
