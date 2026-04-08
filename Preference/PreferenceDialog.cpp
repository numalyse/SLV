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
    m_tabWidget->addTab(m_interfaceTab, prefManager.getText("pref_dialog_tab_interface"));

    m_shortcutTab = new ShortcutTab(m_tabWidget);
    m_tabWidget->addTab(m_shortcutTab, prefManager.getText("pref_dialog_tab_shortcut"));

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
    if(m_shortcutTab->needSave()) {
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

void PreferenceDialog::save(){
    m_shortcutTab->save();
}

void PreferenceDialog::discard(){
    m_shortcutTab->discard();
}