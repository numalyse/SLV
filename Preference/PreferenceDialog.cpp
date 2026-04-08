#include "PreferenceDialog.h"

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
    auto& prefManager = PrefManager::instance();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);

    m_interfaceTab = new InterfaceTab(m_tabWidget);
    m_tabWidget->addTab(m_interfaceTab, prefManager.getText("dialog_preference_tab_interface"));

    m_shortcutTab = new ShortcutTab(m_tabWidget);
    m_tabWidget->addTab(m_shortcutTab, prefManager.getText("dialog_preference_tab_shortcut"));

    mainLayout->addWidget(m_tabWidget);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);

    buttonBox->button(QDialogButtonBox::Save)->setText(prefManager.getText("generic_btn_save"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(prefManager.getText("generic_btn_cancel"));

    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferenceDialog::save); 
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PreferenceDialog::discard); 
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
    return m_shortcutTab->needSave() || m_interfaceTab->needSave();
}

void PreferenceDialog::save(){
    if(!needSave()) return;

    m_shortcutTab->save();
    m_interfaceTab->save();
    showWarning();
}

void PreferenceDialog::discard(){
    m_shortcutTab->discard();
    m_interfaceTab->discard();
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