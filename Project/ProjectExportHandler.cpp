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
#include <QPdfWriter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QImage>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QPageSize>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QTemporaryDir>

#include <memory>
#include <optional>

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
        
            QString fileName = dstPath + QDir::separator() + "TagImage" + QString::number(currentShot+1) + '_' + timeString + ".png";

            bool success = cv::imwrite(fileName.toLocal8Bit().constData(), imgData.img, pngParams);
            if(!success){
                qDebug() << "Impossible de sauvegarder la tag image du plan : " << currentShot;
            }
            ++currentShot;
        }

        return true;

    }

   bool exportToPDF(const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, 
            imageQueue.get(), 
            shots, 
            nullptr, 
            std::optional<int>(cv::COLOR_BGR2RGB), 
            std::optional<cv::Size>({400, 400})
        );

        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

        QString finalPath = dstPath + ".pdf";
        QPdfWriter pdfWriter(finalPath);
        pdfWriter.setPageSize(QPageSize(QPageSize::A4));
        pdfWriter.setResolution(300); 

        QTextDocument doc;
        QTextCursor cursor(&doc);

        QTextCharFormat titleFormat;
        titleFormat.setFontPointSize(24);
        titleFormat.setFontWeight(QFont::Bold);
        titleFormat.setForeground(Qt::red);

        QTextCharFormat subtitleFormat;
        subtitleFormat.setFontPointSize(14);
        subtitleFormat.setFontWeight(QFont::DemiBold);
        subtitleFormat.setForeground(Qt::blue);

        QTextCharFormat normalFormat;
        normalFormat.setFontPointSize(12);
        normalFormat.setForeground(Qt::black);

        QTextBlockFormat titleAlignment;
        titleAlignment.setAlignment(Qt::AlignCenter);
        titleAlignment.setTopMargin(40);
        titleAlignment.setBottomMargin(20);

        QTextBlockFormat leftAlignment;
        leftAlignment.setAlignment(Qt::AlignLeft);

        QTextBlockFormat centerAlignment;
        centerAlignment.setAlignment(Qt::AlignCenter);

        QTextBlockFormat noteAlignment = leftAlignment;
        noteAlignment.setTopMargin(5);

        QTextBlockFormat imageAlignment = centerAlignment;
        imageAlignment.setTopMargin(10);
        imageAlignment.setBottomMargin(15);

        cursor.insertBlock(titleAlignment);
        cursor.insertText("Étude cinématographique", titleFormat);
        
        ImgData imgData{};
        int totalShots = shots.size();
        int currentShot = 0;

        while(true) {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;

            if (progressCallback && totalShots > 0) {
                int percent = static_cast<int>(((currentShot + 1) * 100.0) / totalShots);
                if (!progressCallback(percent)) {
                    decodeThread->requestInterruption();
                    return false;  
                }
            }

            auto& shot = shots[currentShot];
            QString start = TimeFormatter::msToHHMMSSFF(shot.start, fps);
            QString shotDuration = TimeFormatter::msToHHMMSSFF(shot.end - shot.start, fps);

            QString planHeader = QString("- [Plan %1] %2 -> Début : %3 / Durée : %4")
                                        .arg(currentShot + 1).arg(shot.title).arg(start).arg(shotDuration);
            
            cursor.insertBlock(leftAlignment);
            cursor.insertText(planHeader, subtitleFormat);

            if (!shot.note.isEmpty()) {
                cursor.insertBlock(noteAlignment);
                cursor.insertText(shot.note, normalFormat);
            }
            
            // insertion de l'image, déjà à la bonne taille et au bon format dans video decode
            if (!imgData.img.empty()) {
                QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_RGB888);
                QImage safeImage = tempImage.copy();

                QString imgName = QString("img_%1.png").arg(currentShot + 1);
                doc.addResource(QTextDocument::ImageResource, QUrl(imgName), safeImage);
                
                cursor.insertBlock(imageAlignment);

                QTextImageFormat imgFormat;
                imgFormat.setName(imgName);
                cursor.insertImage(imgFormat);
            }

            ++currentShot;
        }

        doc.print(&pdfWriter);
        return true;
    }


    /// @brief Utilise des scripts python pour exporter au format DOCX / PPTX, return false si le type est différent de ces deux
    /// @param type 
    /// @param shots 
    /// @param fps 
    /// @param duration 
    /// @param mediaPath 
    /// @param dstPath 
    /// @param progressCallback 
    /// @return 
    bool exportPython(ExportType type ,const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if (type != ExportType::DOCX && type != ExportType::PPTX) return false;
        QString pythonScriptPath;
        cv::Size imgSize;

        switch (type)
        {
        case ExportType::PPTX:
            pythonScriptPath = "pyScripts/export_pptx.py";
            imgSize = {800,800};
            break;
        case ExportType::DOCX:
        default:
            pythonScriptPath = "pyScripts/export_docx.py";
            imgSize = {400,400};
            break;
        }

        // Dossier temp pour le json et les tag images
        QTemporaryDir tempDir;
        if (!tempDir.isValid()) return false;
        QString tempPath = tempDir.path();

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(10));
        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, imageQueue.get(), shots, nullptr, 
            std::optional<int>(cv::COLOR_BGR2RGB), std::optional<cv::Size>(imgSize)
        );

        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

        ImgData imgData{};
        int totalShots = shots.size();
        int currentShot = 0;

        while(true) {
            imageQueue->waitPop(imgData);
            if(imgData.isFinished) break;

            if (currentShot < totalShots) {
                // Sauvegarde de l'image
                if (!imgData.img.empty()) {
                    QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_RGB888);
                    tempImage.save(tempPath + QString("/image_shot_%1.png").arg(currentShot), "PNG");
                }

                if (progressCallback && totalShots > 0) { 
                    int percent = static_cast<int>(((currentShot + 1) * 95.0) / totalShots); // On va de 0 à 95% car c'est le plus long 
                    if (!progressCallback(percent)) {
                        decodeThread->requestInterruption();
                        return false;  
                    }
                }
                currentShot++;
            }
        }

        // Preparation json
        QJsonArray jsonShots;
        for (int i = 0; i < shots.size(); ++i) {
            QJsonObject shotObj;
            shotObj["id"] = i;
            shotObj["title"] = shots[i].title;
            shotObj["start"] = shots[i].start;
            shotObj["duration"] = shots[i].end - shots[i].start; 
            shotObj["note"] = shots[i].note;
            shotObj["image"] = QString("image_shot_%1.png").arg(i);
            jsonShots.append(shotObj);
        }

        QJsonObject rootData;
        rootData["fps"] = fps;
        rootData["duration"] = duration;
        rootData["dstPath"] = dstPath;
        rootData["tempDir"] = tempPath;
        rootData["shots"] = jsonShots;

        QJsonDocument doc(rootData);
        QFile jsonFile(tempPath + "/export_data.json");
        if (jsonFile.open(QIODevice::WriteOnly)) {
            jsonFile.write(doc.toJson());
            jsonFile.close();
        }

        QProcess pythonProcess;
        pythonProcess.setProcessChannelMode(QProcess::MergedChannels); // Pour lire les prints normaux et les erreurs

        // On passe le chemin du JSON
        pythonProcess.start("py", QStringList() << pythonScriptPath << jsonFile.fileName());

        // Boucle d'attente active pour lire la progression en temps réel
        while (pythonProcess.waitForReadyRead(-1)) {
            while (pythonProcess.canReadLine()) {
                QString line = QString::fromUtf8(pythonProcess.readLine()).trimmed();
                
                if (line.startsWith("PROGRESS:")) {
                    int pyPercent = line.mid(9).toInt(); // Python envoie de 0 à 100
                    int totalPercent = 95 + static_cast<int>(pyPercent * 0.05); // Remap de 95 à 100
                    
                    if (progressCallback && !progressCallback(totalPercent)) {
                        pythonProcess.kill();
                        return false;
                    }
                } else {
                    qDebug() << "[PYTHON]:" << line; 
                }
            }
        }

        pythonProcess.waitForFinished(-1);

        return (pythonProcess.exitCode() == 0);
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