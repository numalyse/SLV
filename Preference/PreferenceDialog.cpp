#include "PreferenceDialog.h"

#include "Preference/BasePreferenceTab.h"
#include "Preference/Tabs/InterfaceTab.h"
#include "Preference/Tabs/ShortcutTab.h"
#include "Preference/Tabs/PathTab.h"

#include "PrefManager.h"
#include "GenericDialog.h"

#include <QFormLayout>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton> 

PreferenceDialog::PreferenceDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setMinimumWidth(500);

    auto& prefManager = PrefManager::instance();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);

    InterfaceTab* interfaceTab = new InterfaceTab(m_tabWidget);
    m_tabWidget->addTab(interfaceTab, prefManager.getText("dialog_preference_tab_interface"));
    m_tabs.append(interfaceTab);
    connect(interfaceTab, &BasePreferenceTab::tabChanges, this, &PreferenceDialog::toggleButtons);

    ShortcutTab* shortcutTab = new ShortcutTab(m_tabWidget);
    m_tabWidget->addTab(shortcutTab, prefManager.getText("dialog_preference_tab_shortcut"));
    m_tabs.append(shortcutTab);
    connect(shortcutTab, &BasePreferenceTab::tabChanges, this, &PreferenceDialog::toggleButtons);

    PathTab* pathTab = new PathTab(m_tabWidget);
    m_tabWidget->addTab(pathTab, prefManager.getText("dialog_preference_tab_paths"));
    m_tabs.append(pathTab);
    connect(pathTab, &BasePreferenceTab::tabChanges, this, &PreferenceDialog::toggleButtons);

    mainLayout->addWidget(m_tabWidget);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);

    m_buttonBox->button(QDialogButtonBox::Save)->setText(prefManager.getText("generic_btn_save"));
    m_buttonBox->button(QDialogButtonBox::Cancel)->setText(prefManager.getText("generic_btn_cancel"));

    m_buttonBox->setDisabled(true);

    mainLayout->addWidget(m_buttonBox);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &PreferenceDialog::save); 
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &PreferenceDialog::discard); 
}

void PreferenceDialog::closeEvent(QCloseEvent *event)
{
    auto& prefManager = PrefManager::instance();
    if(needSave()) {
        SLV::showGenericDialog(
            nullptr, 
            prefManager.getText("dialog_preference_save_tilte"),
            prefManager.getText("dialog_preference_save_text"),

            [this, event](){
                this->save();
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

bool PreferenceDialog::needSave(){
    for(auto& tab : m_tabs){
        if (tab->needSave()) return true;
    }
    return false;
}

void PreferenceDialog::save(){
    if(!needSave()) return;

    for(auto& tab : m_tabs){
        tab->save();
    }
    showWarning();
    m_buttonBox->setDisabled(true);
}

void PreferenceDialog::discard(){
    for(auto& tab : m_tabs){
        tab->discard();
    }
    m_buttonBox->setDisabled(true);
}

void PreferenceDialog::showWarning(){
    auto& prefManager = PrefManager::instance();
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(prefManager.getText("dialog_preference_restart_to_apply_tittle"));
    msgBox.setText(prefManager.getText("dialog_preference_restart_to_apply_text"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void PreferenceDialog::toggleButtons(){
    if(!needSave()){
        m_buttonBox->setDisabled(true);
    }else {
        m_buttonBox->setDisabled(false);
    }
}