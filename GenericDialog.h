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
        const QString& textInfo,
        const std::function<void()>& onYes,        
        const std::function<void()>& onNo = nullptr,        
        const std::function<void()>& onCancel = nullptr 
    ) {
        PrefManager& prefManager = PrefManager::instance();

        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle(title);
        msgBox.setText(text);
        
        if (!textInfo.isEmpty()) {
            msgBox.setInformativeText(textInfo);
        }

        QPushButton *yesBtn = msgBox.addButton(prefManager.getText("generic_dialog_btn_yes"), QMessageBox::YesRole);
        QPushButton *noBtn = nullptr;
        if(onNo){
            noBtn = msgBox.addButton(prefManager.getText("generic_dialog_btn_no"), QMessageBox::NoRole);
        }

        QPushButton *cancelBtn = msgBox.addButton(prefManager.getText("generic_dialog_btn_cancel"), QMessageBox::RejectRole);

        msgBox.exec();

        if (msgBox.clickedButton() == yesBtn) {
            if (onYes) onYes();
        } else if (noBtn && msgBox.clickedButton() == noBtn) {
            if (onNo) onNo();
        } else {
            if (onCancel) onCancel();
        }
    }

    inline void showGenericDialog(
        QWidget* parent,
        const QString& title,
        const QString& text,
        const std::function<void()>& onYes,
        const std::function<void()>& onNo = nullptr,
        const std::function<void()>& onCancel = nullptr
    ) {
        showGenericDialog(parent, title, text, QString(), onYes, onNo, onCancel);
    }


} // namespace SLV

#endif
