#include "Project/ProjectExportHandler.h"
#include "ProjectExportHandler.h"
#include "TextManager.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <optional>

namespace ProjectExportHandler {
    
    std::optional<ExportType> selectFormatWindow(const QString& originalFormat) {
        QDialog dialog;
        auto& txtManager = TextManager::instance();
        dialog.setWindowTitle(txtManager.get("export_format_selection_title")); 
        dialog.setMinimumWidth(300);

        QVBoxLayout* layout = new QVBoxLayout(&dialog);

        QLabel* label = new QLabel(txtManager.get("export_format_selection_txt"), &dialog);
        layout->addWidget(label);

        QComboBox* comboBox = new QComboBox(&dialog);
        
        comboBox->addItem(".txt", static_cast<int>(ExportType::TXT));
        comboBox->addItem(".pdf", static_cast<int>(ExportType::PDF));
        comboBox->addItem(".pptx", static_cast<int>(ExportType::PPTX));
        comboBox->addItem(".docx", static_cast<int>(ExportType::DOCX));
        if(originalFormat != ".mp4") comboBox->addItem(".mp4", static_cast<int>(ExportType::MP4)); // si on est deja en mp4, on n'affiche pas l'option mp4
        comboBox->addItem(originalFormat, static_cast<int>(ExportType::SRC));
        comboBox->addItem(txtManager.get("export_format_selection_txt_tagImage"), static_cast<int>(ExportType::TagImage));
        
        layout->addWidget(comboBox);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        
        buttonBox->button(QDialogButtonBox::Ok)->setText(txtManager.get("generic_dialog_btn_yes"));
        buttonBox->button(QDialogButtonBox::Cancel)->setText(txtManager.get("generic_dialog_btn_cancel"));
        
        layout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            int selectedValue = comboBox->currentData().toInt();
            qDebug() << "Format choisi : " << selectedValue;
            return static_cast<ExportType>(selectedValue);
        }

        return std::nullopt; 
    }

    bool ProjectExportHandler::exportProject(const Project *project, TimelineWidget *timeline, ExportType type, const QString &destinationPath)
    {
        return false;
    }

}