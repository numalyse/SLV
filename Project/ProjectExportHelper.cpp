#include "Project/ProjectExportHelper.h"
#include "ProjectExportHelper.h"
#include "PrefManager.h"
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
#include <QStringList>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>

#include <memory>
#include <optional>
#include <utility>
#include <algorithm>
namespace  {

    int findShotIndexAtTime(const QVector<Shot>& shots, int64_t timeMs) {
        for (int IShot = 0; IShot < shots.size(); ++IShot) {
            if(shots[IShot].start <= timeMs && shots[IShot].end >= timeMs) return IShot;
        }
        return -1;
    }



    std::pair<int, int> computeFontSizeAndSpacing(int width, int height, double ratio = 0.02) {
        int calculatedSize = static_cast<int>(height * ratio);
        int fontSize = std::max(12, calculatedSize);
        
        int lineSpacing = static_cast<int>(fontSize * 1.5); 
        
        return {fontSize, lineSpacing}; 
    }

    /// @brief Sépare le texte en plusieurs lignes si le texte est trop grand
    /// @param text 
    /// @param maxChars 
    /// @return 
    QStringList wrapText(const QString& text, int maxChars) {
        QStringList result;
        
        if (text.length() <= maxChars) {
            result.append(text);
            return result;
        }

        // On découpe la phrase en mots en ignorant les espaces multiples
        QStringList words = text.split(' ', Qt::SkipEmptyParts);
        QString currentLine;

        for (const QString& word : words) {
            if (currentLine.isEmpty()) {
                currentLine = word; // Premier mot de la ligne
            } else if (currentLine.length() + 1 + word.length() <= maxChars) {
                currentLine += " " + word; // On ajoute le mot s'il y a de la place
            } else {
                result.append(currentLine); // La ligne est pleine, on la sauvegarde
                currentLine = word;         // On commence une nouvelle ligne
            }
        }
        
        if (!currentLine.isEmpty()) {
            result.append(currentLine);
        }

        return result;
    }

    /// @brief Calcule le nombre de char max possible sur une ligne puis sépare les lignes de texte si nécessaire
    /// @param shotTitleTxt 
    /// @param timecodeTxt 
    /// @param noteTxt 
    /// @param maxWidth 
    /// @param fontSize 
    /// @return 
    QStringList formatText(        
        const QString& shotTitleTxt,  
        const QString& timecodeTxt,  
        const QString& noteTxt,
        int maxWidth,
        int fontSize
    ){

        QFont font("Arial");
        font.setPixelSize(fontSize); 
        font.setBold(true);

        QFontMetrics fm(font);

        int charWidth = fm.averageCharWidth();
        if (charWidth == 0) charWidth = 10;

        int maxCharsPerLine = (maxWidth - 100) / charWidth; 
        if (maxCharsPerLine <= 0) maxCharsPerLine = 10;

        QStringList linesToDraw({shotTitleTxt + " - " + timecodeTxt});

        QStringList noteLines = noteTxt.split("\n");
        for( auto& noteLine : noteLines){
            linesToDraw.push_back(noteLine.replace("\t", "   "));
        }

        QStringList wrappedLines{};
        for ( auto& line : linesToDraw ) {
            wrappedLines.append(wrapText(line, maxCharsPerLine));
        }

        return wrappedLines;
    }

    /// @brief Utilise une QImage et un QPainter pour écrire. Ecrit directement dans les données de la frame passée en paramètre 
    /// @param frame 
    /// @param wrappedLines 
    /// @param fontSize 
    /// @param lineSpacing 
    void writeOnOverlay(
        QImage& overlay,
        const QStringList& wrappedLines,
        int fontSize,
        int lineSpacing
    ){

        QPainter painter(&overlay);
        painter.setRenderHint(QPainter::Antialiasing); 

        QFont font("Arial");
        font.setPixelSize(fontSize);
        font.setBold(true);
        painter.setFont(font);

        int outlineThickness = std::max(2, fontSize / 10); 
        QPen outlinePen(Qt::black, outlineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QBrush textBrush(Qt::white);

        int currentY = overlay.height() * 0.01 + lineSpacing ; 
        int startX = overlay.width() * 0.02;

        for (const auto& line : wrappedLines) {
            QPainterPath path;
            
            path.addText(startX, currentY, font, line);

            // Contour noir
            painter.setPen(outlinePen);
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(path);

            // Intérieur en blanc
            painter.setPen(Qt::NoPen);
            painter.setBrush(textBrush);
            painter.drawPath(path);

            currentY += lineSpacing;
        }

        painter.end();
    }



    bool mergeVideoAndAudio(const QString& originalMediaPath, const QString& tempVideoPath, const QString& finalFilePath) {
        QProcess ffmpegProcess;
        QStringList arguments;

        // Ecraser le fichier de sortie s'il existe déjà
        arguments << "-y"; 

        // Entrée 0 : La vidéo temporaire sans son 
        arguments << "-i" << tempVideoPath;

        // Entrée 1 : La vidéo/audio d'origine 
        arguments << "-i" << originalMediaPath;

        // Commence depuis le début des des vidéos
        arguments << "-map" << "0:v:0";
        arguments << "-map" << "1:a:0";

        // On garde le codec video de la video temporaire et le son de l'original
        arguments << "-c:v" << "copy";
        arguments << "-c:a" << "copy";

        // Coupe à la fin du plus court ?
        // arguments << "-shortest";

        arguments << finalFilePath;

        qDebug() << "[export_video] Démarrage du mixage FFmpeg...";

        ffmpegProcess.start("ffmpeg", arguments);

        if (!ffmpegProcess.waitForStarted()) {
            qCritical() << "Impossible de lancer FFmpeg.";
            return false;
        }

        ffmpegProcess.waitForFinished(-1);

        if (ffmpegProcess.exitCode() != 0) {
            qCritical() << "Erreur lors du mixage FFmpeg :" << ffmpegProcess.readAllStandardError();
            return false;
        }

        qDebug() << "[export_video] Écriture faite avec succès : " << finalFilePath;
        return true;
    }

}

namespace ProjectExportHelper {
    

    bool exportToTxt(const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

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
        if(progressCallback) progressCallback(0);

        QDir folder(dstPath);
        if( folder.exists() ) {
            folder.removeRecursively();
        }

        if (!QDir().mkpath(dstPath)) {
            qDebug() << "Erreur : Impossible de créer le dossier " << dstPath;
            return false;
        }

        ImgData imgData{};

        int totalShots = shots.size();
        int currentShot = 0;

        std::vector<int> pngParams;
        pngParams.push_back(cv::IMWRITE_PNG_COMPRESSION);
        pngParams.push_back(3);

        QFileInfo mediaFileInfo(mediaPath);
        QString mediaName = mediaFileInfo.baseName();

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(mediaPath, imageQueue.get(), shots);
        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

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
        
            QString fileName = QDir(dstPath).filePath("TagImage" + QString::number(currentShot+1) + '_' + timeString + ".png");

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
        if(progressCallback) progressCallback(0);
        
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

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, 
            imageQueue.get(), 
            shots, 
            nullptr, 
            std::optional<int>(), 
            std::optional<cv::Size>({400, 400})
        );

        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

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
                QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_BGR888);
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
    /// @return 
    bool exportPython(ExportType type ,const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if (type != ExportType::DOCX && type != ExportType::PPTX) return false;
        
        if(progressCallback) progressCallback(0);
        
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

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));
        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, imageQueue.get(), shots, nullptr, 
            std::optional<int>(), std::optional<cv::Size>(imgSize)
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
                    QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_BGR888);
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


    bool exportVideo(ExportType type, const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

        QFileInfo mediaInfo(mediaPath);
        QString extension = mediaInfo.suffix();
        if (type == ExportType::MP4) extension = "mp4";

        QTemporaryDir tempDir;
        if (!tempDir.isValid()) return false;
        QString tempDirPath = tempDir.path();
        QString tempVideo = QDir(tempDirPath).filePath("temp_video." + extension);
        
        cv::VideoCapture cap(mediaPath.toStdString(), cv::CAP_FFMPEG);

        int currentFrame = 0;
        int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);

        if (!cap.isOpened()) {
            qCritical() << "Impossible de lire la video pour exporter";
            return false;
        }

        // On force en mp4 si l'utilisateur veut un export mp4 sinon c'est le même que la source
        int fourcc = (type == ExportType::MP4) ? cv::VideoWriter::fourcc('m', 'p', '4', 'v') : static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));

        cv::Size originalSize(
            static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)),
            static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT))
        );

        cap.release();

        cv::VideoWriter writer(
            tempVideo.toLocal8Bit().constData(),
            fourcc,                      
            fps,                         
            originalSize 
        );

        if (!writer.isOpened()) {
            qDebug() << "Le codec n'est pas supporté pour l'écriture. Utilisation de mp4v...";
            
            // On tente mp4v si le codec ne fonctionne pas
            int fallbackFourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
            writer.open(
                tempVideo.toLocal8Bit().constData(), 
                fallbackFourcc, 
                fps, 
                originalSize
            );
            if(!writer.isOpened()){
                qDebug() << "Impossible d'écrire";
                return false;
            }else qDebug() << "mp4v Ok";
        }

        int currentShot = -1;
        int64_t endShotTime = -1;

        ImgData imgData;
        QStringList wrappedText;

        int percent = 0;

        auto [fontSize, lineSpacing] = computeFontSizeAndSpacing(originalSize.width, originalSize.height, 0.020);

        QImage textOverlay(originalSize.width, originalSize.height, QImage::Format_ARGB32_Premultiplied);

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));
        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, imageQueue.get(), {}
        );

        QObject::connect(decodeThread, &QThread::finished, decodeThread, &QObject::deleteLater);
        decodeThread->start();

        while ( true )
        {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;
            if(imgData.img.empty()) continue;

            // Si endShotTime < timeMs => opencv a dépassé la fin du plan, on met à jour le plan courant
            if( endShotTime < imgData.timeMs || currentShot == -1){
                 // Récupère le temps et l'id du plan comprenant imgData.timeMs
                currentShot = findShotIndexAtTime(shots, imgData.timeMs);
                if(currentShot == -1){ // Si pas de temps trouvé, le text devient vide
                    qDebug() << "Impossible de trouver un plan qui comprends : " << imgData.timeMs << "garde le textPrecende";
                    wrappedText.clear();
                    textOverlay.fill(Qt::transparent); 
                }else { // Le texte est mis à jour avec les infos du nouveau plan
                    auto& s = shots[currentShot];
                    endShotTime = s.end;
                    QString shotTitleTxt = "[Plan " + QString::number(currentShot+1) + "] " + s.title;
                    QString timecodeTxt = "Début : " + TimeFormatter::msToHHMMSSFF(s.start, fps) + " / Durée : " + TimeFormatter::msToHHMMSSFF(s.end - s.start, fps);
                    QString noteTxt = s.note;
                    wrappedText = formatText(shotTitleTxt, timecodeTxt, noteTxt, originalSize.width, fontSize);
                    textOverlay.fill(Qt::transparent); 
                    writeOnOverlay(textOverlay, wrappedText, fontSize, lineSpacing); // Mise à jour de l'overlay à chaque fois que le plan change
                }

            }

            QImage img(imgData.img.data, imgData.img.cols, imgData.img.rows, static_cast<int>(imgData.img.step), QImage::Format_BGR888);
            QPainter painter(&img);
            painter.drawImage(0, 0, textOverlay); // Draw l'image transparent avec le texte par dessus l'image (rapide)
            writer.write(imgData.img);

            if (progressCallback && totalFrames > 0) {
                int percent = static_cast<int>(((currentFrame + 1) * 100.0) / totalFrames);
                if( !progressCallback(percent)){
                    writer.release();
                    return false;
                }
            }
            
            ++currentFrame;

        }

        writer.release();

        if( currentFrame != totalFrames){
            qWarning() << "La vidéo lue contenait" << currentFrame << "images au lieu des" << totalFrames << "annoncées par opencv. La vidéo a tout de même été exportée.";
        }

        if( progressCallback && !progressCallback(percent)){
            return false;
        }

        QString finalVideoPath = dstPath + '.' + extension; 
        mergeVideoAndAudio(mediaPath, tempVideo, finalVideoPath);

        return true;
    }

    std::optional<ExportType> selectFormatWindow(const QString &originalFormat)
    {
        QDialog dialog;
        auto& txtManager = PrefManager::instance();
        dialog.setWindowTitle(txtManager.getText("export_format_selection_title")); 
        dialog.setMinimumWidth(300);

        QVBoxLayout* layout = new QVBoxLayout(&dialog);

        QLabel* label = new QLabel(txtManager.getText("export_format_selection_txt"), &dialog);
        layout->addWidget(label);

        QComboBox* comboBox = new QComboBox(&dialog);
        
        comboBox->addItem(".txt", static_cast<int>(ExportType::TXT));
        comboBox->addItem(".pdf", static_cast<int>(ExportType::PDF));
        comboBox->addItem(".pptx", static_cast<int>(ExportType::PPTX));
        comboBox->addItem(".docx", static_cast<int>(ExportType::DOCX));
        if(originalFormat != ".mp4") comboBox->addItem(".mp4", static_cast<int>(ExportType::MP4)); // si on est deja en mp4, on n'affiche pas l'option mp4
        comboBox->addItem(originalFormat, static_cast<int>(ExportType::SRC));
        comboBox->addItem(txtManager.getText("export_format_selection_txt_tagImage"), static_cast<int>(ExportType::TagImage));
        
        layout->addWidget(comboBox);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        
        buttonBox->button(QDialogButtonBox::Ok)->setText(txtManager.getText("generic_dialog_btn_yes"));
        buttonBox->button(QDialogButtonBox::Cancel)->setText(txtManager.getText("generic_dialog_btn_cancel"));
        
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