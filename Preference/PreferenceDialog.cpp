#include "PreferenceDialog.h"

#include "PrefManager.h"
#include "GenericDialog.h"

#include <QFormLayout>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>

PreferenceDialog::PreferenceDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    auto& prefManager = PrefManager::instance();
    QFormLayout* prefManagerLayout = new QFormLayout(this);
    m_tabWidget = new QTabWidget(this);

    m_interfaceTab = new InterfaceTab(m_tabWidget);
    m_tabWidget->addTab(m_interfaceTab, prefManager.getText("pref_dialog_tab_interface"));


    m_shortcutTab = new ShortcutTab(m_tabWidget);
    m_tabWidget->addTab(m_shortcutTab, prefManager.getText("pref_dialog_tab_shortcut"));

    prefManagerLayout->addWidget(m_tabWidget);
    
}

void PreferenceDialog::closeEvent(QCloseEvent *event)
{
    auto& prefManager = PrefManager::instance();
    if(m_shortcutTab->needSave()) {
        SLV::showGenericDialog(
            nullptr, 
            prefManager.getText("dialog_preference_save_tilte"),
            prefManager.getText("dialog_preference_save_text"),

            [this, event](){
                m_shortcutTab->save();
                event->accept();
            },

            [event](){
                event->accept();
            },

            [event](){
                event->ignore();
            }

        );
    }else {
        event->accept();
    }
}
