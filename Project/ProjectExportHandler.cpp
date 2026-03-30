#include "Project/ProjectExportHandler.h"
#include "ProjectExportHandler.h"
#include "TextManager.h"
#include "TimeFormatter.h"
#include "TSQueue.h"
#include "DecodeThread.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <optional>
#include <functional>
#include <QDir>
#include <QObject>
#include <QFileInfo>

#include <memory>

namespace ProjectExportHandler {
    

    bool exportToTxt(const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        QFile file(dstPath + ".txt");

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        out << "=== Etude cinematographique ===\n\n";
        out << "Nombre total de plans : " << shots.size() << "\n\n";

        int totalShots = shots.size();

        for (int IShot = 0; IShot < shots.size(); ++IShot) {
            const Shot &shot = shots[IShot];

            int64_t shotDuration = shot.end - shot.start;

            QString timeStr = TimeFormatter::msToHHMMSSFF(shot.start, fps);
            QString endStr = TimeFormatter::msToHHMMSSFF(shotDuration, fps);

            out << "- [Plan " << (IShot + 1) << "] " << shot.title 
                << " -> Debut : " << timeStr << " / Duree : " << endStr << "\n";

            if (!shot.note.trimmed().isEmpty()) {
                out << shot.note.trimmed() << "\n"; 
            }

            out << "\n";

            if (progressCallback && totalShots > 0) {
                int percent = static_cast<int>(((IShot + 1) * 100.0) / totalShots);

                if (!progressCallback(percent)) {
                    file.close();
                    file.remove(); 
                    return false;  
                }

            }
        }

        file.close();
        return true;
    }

    bool exportToTagImage(const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        QDir folder(dstPath);
        if( folder.exists() ) {
            folder.removeRecursively();
        }

        if (!QDir().mkpath(dstPath)) {
            qDebug() << "Erreur : Impossible de créer le dossier " << dstPath;
            return false;
        }

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(mediaPath, imageQueue.get(), shots);
        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

        ImgData imgData{};

        int totalShots = shots.size();
        int currentShot = 0;

        std::vector<int> pngParams;
        pngParams.push_back(cv::IMWRITE_PNG_COMPRESSION);
        pngParams.push_back(3);

        QFileInfo mediaFileInfo(mediaPath);
        QString mediaName = mediaFileInfo.baseName();

        while(true) {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;

            if (progressCallback && totalShots > 0) {
                int percent = static_cast<int>(((currentShot + 1) * 100.0) / totalShots);

                if (!progressCallback(percent)) {
                    folder.removeRecursively();
                    return false;  
                }

            }
            
            QString timeString = TimeFormatter::msToHHMMSSFF(shots[currentShot].tagImageTime, fps);
            timeString.replace(":", "-");
        
            QString fileName = dstPath + QDir::separator() + "TagImage" + QString::number(currentShot) + '_' + timeString + ".png";

            bool success = cv::imwrite(fileName.toLocal8Bit().constData(), imgData.img, pngParams);
            if(!success){
                qDebug() << "Impossible de sauvegarder la tag image du plan : " << currentShot;
            }
            ++currentShot;
        }

        return true;

    }

    std::optional<ExportType> selectFormatWindow(const QString &originalFormat)
    {
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
}