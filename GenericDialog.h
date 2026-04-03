#ifndef GENERICDIALOG_H
#define GENERICDIALOG_H

#include "PrefManager.h"

#include <QMessageBox>
#include <QPushButton>
#include <functional>
#include <QString>

namespace SLV
{
    
    inline void showGenericDialog(
        QWidget* parent,
        const QString& title,
        const QString& text,
        std::function<void()> onYes,        
        std::function<void()> onNo = nullptr,        
        std::function<void()> onCancel = nullptr 
    ) {


        PrefManager& txtManager = PrefManager::instance();

        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle(title);
        msgBox.setText(text);

        QPushButton *yesBtn = msgBox.addButton(txtManager.getText("generic_dialog_btn_yes"), QMessageBox::YesRole);
        QPushButton *noBtn = nullptr;
        if(onNo){
            noBtn = msgBox.addButton(txtManager.getText("generic_dialog_btn_no"), QMessageBox::NoRole);
        }

        QPushButton *cancelBtn = msgBox.addButton(txtManager.getText("generic_dialog_btn_cancel"), QMessageBox::RejectRole);

        msgBox.exec();

        if (msgBox.clickedButton() == yesBtn) {
            if (onYes) onYes();
        } else if (msgBox.clickedButton() == noBtn) {
            if (onNo) onNo();
        } else {
            if (onCancel) onCancel();
        }
    }

} // namespace SLV

#endif
