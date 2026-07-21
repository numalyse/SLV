#include "Project/ProjectExportHelper.h"
#include "ProjectExportHelper.h"
#include "VideoCaptureHelper.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "TSQueue.h"
#include "DecodeThread.h"
#include "Media.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
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

    int findItemIndexAtTime(const QVector<ExportItem>& items, int64_t timeMs) {
        for (int IItem = 0; IItem < items.size(); ++IItem) {
            if(items[IItem].start <= timeMs && items[IItem].end >= timeMs) return IItem;
        }
        return -1;
    }

    /// @brief Retrieve timecodes to decode from ExportItems 
    QVector<int64_t> toImageTimes(const QVector<ExportItem>& items) {
        QVector<int64_t> imageTimes;
        imageTimes.reserve(items.size());
        for (const auto& item : items) {
            imageTimes.push_back(item.imageTime);
        }
        return imageTimes;
    }

    /// @brief Stops a decode thread and its queue, waits for the thread to finish, then destroys it
    void stopDecodeThread(DecodeThread* decodeThread, TSQueue<ImgData>* imageQueue) {
        decodeThread->requestInterruption();
        imageQueue->stop();
        decodeThread->wait();
        delete decodeThread;
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

        QString appDir = QCoreApplication::applicationDirPath();
        QString ffmpegExe;
#if defined(Q_OS_WIN)
        ffmpegExe = appDir + "/bin/ffmpeg.exe";
#elif defined(Q_OS_MAC)
        ffmpegExe = appDir + "/../Resources/ffmpeg/ffmpeg";
    #else
        ffmpegExe = appDir + "/bin/ffmpeg";
#endif

        ffmpegProcess.start(ffmpegExe, arguments);
        //ffmpegProcess.start(QString(FFMPEG_EXECUTABLE), arguments);

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
    

    ExportLabels makeExportLabels(ExportSource source)
    {
        auto& txtManager = PrefManager::instance();
        ExportLabels labels;

        if (source == ExportSource::Annotations) {
            labels.item      = txtManager.getText("annotation");
            labels.count     = txtManager.getText("number_of_annotations");
            labels.title     = txtManager.getText("annotation_name");
            labels.startTime = txtManager.getText("shot_detail_start_time_name");
            labels.endTime   = txtManager.getText("shot_detail_end_time_name");
            labels.duration  = txtManager.getText("shot_detail_duration_time_name");
            labels.imgTxt    = txtManager.getText("annotation_note");
            labels.soundTxt  = QString(); 
        } else {
            labels.item      = txtManager.getText("shot");
            labels.count     = txtManager.getText("number_of_shots");
            labels.title     = txtManager.getText("shot_detail_title_name");
            labels.startTime = txtManager.getText("shot_detail_start_time_name");
            labels.endTime   = txtManager.getText("shot_detail_end_time_name");
            labels.duration  = txtManager.getText("shot_detail_duration_time_name");
            labels.imgTxt    = txtManager.getText("shot_detail_img_txt_name");
            labels.soundTxt  = txtManager.getText("shot_detail_sound_txt_name");
        }

        return labels;
    }

    QVector<ExportItem> fromShots(const QVector<Shot>& shots)
    {
        QVector<ExportItem> items;
        items.reserve(shots.size());
        for (const auto& shot : shots) {
            items.push_back({shot.title, shot.start, shot.end, shot.tagImageTime, shot.imgTxt, shot.soundTxt});
        }
        return items;
    }

    QVector<ExportItem> fromAnnotations(const QVector<Annotation>& annotations)
    {
        QVector<ExportItem> items;
        items.reserve(annotations.size());
        for (const auto& annot : annotations) {
            items.push_back({annot.name, annot.start, annot.end, annot.start, annot.note, QString()});
        }
        return items;
    }

    bool exportToTxt(const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

        QFile file(dstPath + ".txt");

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        int totalItems = items.size();

        out << "=== " << QFileInfo(mediaPath).baseName() << " ===\n\n";
        out << labels.count << " : " << totalItems << "\n\n";

        for (int IItem = 0; IItem < totalItems; ++IItem) {
            const ExportItem &item = items[IItem];

            int64_t itemDuration = item.end - item.start;

            QString timeStr = TimeFormatter::msToHHMMSSFF(item.start, fps);
            QString durStr = TimeFormatter::msToHHMMSSFF(itemDuration, fps);

            out << "- [" << labels.item << " " << (IItem + 1) << "] " << item.title
                << " -> "<< labels.startTime <<" : " << timeStr
                << " / " << labels.duration <<" : " << durStr << "\n";

            if (!item.imgTxt.trimmed().isEmpty()) {
                out << labels.imgTxt << " : " << item.imgTxt.trimmed() << "\n";
            }

            out << "\n";

            if (!labels.soundTxt.isEmpty() && !item.soundTxt.trimmed().isEmpty()) {
                out << labels.soundTxt << " : " << item.soundTxt.trimmed() << "\n";
            }

            out << "\n";

            if (progressCallback && totalItems > 0) {
                int percent = static_cast<int>(((IItem + 1) * 100.0) / totalItems);
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

    bool exportToTagImage(const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

        QDir folder(dstPath);

        if (!QDir().mkpath(dstPath)) {
            qDebug() << "Erreur : Impossible de créer le dossier " << dstPath;
            return false;
        }

        ImgData imgData{};

        int itemCount = items.size();
        int currItemId = 0;

        std::vector<int> pngParams;
        pngParams.push_back(cv::IMWRITE_PNG_COMPRESSION);
        pngParams.push_back(3);

        QFileInfo mediaFileInfo(mediaPath);
        QString mediaName = mediaFileInfo.baseName();

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(mediaPath, sar, imageQueue.get(), toImageTimes(items));

        decodeThread->start();

        while(true) {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;

            if (progressCallback && itemCount > 0) {
                int percent = static_cast<int>(((currItemId + 1) * 100.0) / itemCount);
                if (!progressCallback(percent)) {
                    // folder.removeRecursively();
                    stopDecodeThread(decodeThread, imageQueue.get());
                    return false;
                }
            }

            QString timeString = TimeFormatter::msToHHMMSSFF(items[currItemId].imageTime, fps);
            timeString.replace(":", "-");
            timeString.replace(".", "-");
        
            QString fileName = QDir(dstPath).filePath("TF" + QString::number(currItemId+1) + '_' + timeString + ".png");

            bool success = cv::imwrite(fileName.toLocal8Bit().constData(), imgData.img, pngParams);
            if(!success){
                qDebug() << "Impossible de sauvegarder la tag image du plan : " << currItemId;
            }
            ++currItemId;
        }

        stopDecodeThread(decodeThread, imageQueue.get());
        return true;

    }

   bool exportToPDF(const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback)
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

        QTextCharFormat labelFormat;
        labelFormat.setFontPointSize(12);
        labelFormat.setForeground(Qt::blue);

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
        cursor.insertText(QFileInfo(mediaPath).baseName(), titleFormat);
        
        ImgData imgData{};
        int itemCount = items.size();
        int currItemId = 0;

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));

        DecodeThread* decodeThread = new DecodeThread(
            mediaPath,
            sar,
            imageQueue.get(),
            toImageTimes(items),
            nullptr,
            std::optional<int>(),
            std::optional<cv::Size>({400, 400})
        );

        decodeThread->start();

        while(true) {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;

            if (progressCallback && itemCount > 0) {
                int percent = static_cast<int>(((currItemId + 1) * 100.0) / itemCount);
                if (!progressCallback(percent)) {
                    stopDecodeThread(decodeThread, imageQueue.get());
                    return false;
                }
            }

            auto& item = items[currItemId];
            QString start = TimeFormatter::msToHHMMSSFF(item.start, fps);
            QString itemDuration = TimeFormatter::msToHHMMSSFF(item.end - item.start, fps);

            QString planHeader = QString("- [%1 %2] %3 -> %4 : %5 / %6 : %7")
                                        .arg(labels.item)
                                        .arg(currItemId + 1)
                                        .arg(item.title)
                                        .arg(labels.startTime)
                                        .arg(start)
                                        .arg(labels.duration)
                                        .arg(itemDuration);
            
            cursor.insertBlock(leftAlignment);
            cursor.insertText(planHeader, subtitleFormat);
            
            // insertion de l'image, déjà à la bonne taille et au bon format dans video decode
            if (!imgData.img.empty()) {
                QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_BGR888);
                QImage safeImage = tempImage.copy();

                QString imgName = QString("img_%1.png").arg(currItemId + 1);
                doc.addResource(QTextDocument::ImageResource, QUrl(imgName), safeImage);
                
                cursor.insertBlock(imageAlignment);

                QTextImageFormat imgFormat;
                imgFormat.setName(imgName);
                cursor.insertImage(imgFormat);
            }

            if (!item.imgTxt.isEmpty()) {
                cursor.insertBlock(noteAlignment);
                cursor.insertText(labels.imgTxt + " : ", labelFormat);
                cursor.insertText(item.imgTxt, normalFormat);
            }

            if (!labels.soundTxt.isEmpty() && !item.soundTxt.isEmpty()) {
                cursor.insertBlock(noteAlignment);
                cursor.insertText(labels.soundTxt + " : ", labelFormat);
                cursor.insertText(item.soundTxt, normalFormat);
            }

            ++currItemId;
        }

        stopDecodeThread(decodeThread, imageQueue.get());

        doc.print(&pdfWriter);
        return true;
    }

    bool exportToCSV(const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

        QString finalPath = dstPath + ".csv";
        QFile file(finalPath);
        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "[EXPORT CSV] Impossible de créer le fichier " << finalPath;
            return false;
        }

        QTextStream out(&file);
        
        out.setGenerateByteOrderMark(true); 

        bool hasSoundColumn = !labels.soundTxt.isEmpty();

        out << '"' << labels.item << '"' << ';'
            << '"' << labels.title << '"' << ';'
            << '"' << labels.startTime << '"' << ';'
            << '"' << labels.endTime << '"' << ';'
            << '"' << labels.duration << '"' << ';'
            << '"' << labels.imgTxt << '"';
        if (hasSoundColumn) {
            out << ';' << '"' << labels.soundTxt << '"';
        }
        out << "\n";

        int itemCount = items.size();

        for (int currItemId = 0; currItemId < itemCount; ++currItemId) {

            if (progressCallback && itemCount > 0) {
                int percent = static_cast<int>(((currItemId + 1) * 100.0) / itemCount);
                if (!progressCallback(percent)) {
                    file.close();
                    return false;
                }
            }

            auto& item = items[currItemId];
            QString start = TimeFormatter::msToHHMMSSFF(item.start, fps);
            QString end = TimeFormatter::msToHHMMSSFF(item.end, fps);
            QString itemDuration = TimeFormatter::msToHHMMSSFF(item.end - item.start, fps);

            // Échappe les champs texte pour respecter le format CSV (guillemets, retours à la ligne, séparateur)
            auto csvField = [](QString text) -> QString {
                text.replace("\"", "\"\"");
                return '"' + text + '"';
            };

            out << (currItemId + 1) << ";"
                << csvField(item.title) << ";"
                << csvField(start) << ";"
                << csvField(end) << ";"
                << csvField(itemDuration) << ";"
                << csvField(item.imgTxt);
            if (hasSoundColumn) {
                out << ";" << csvField(item.soundTxt);
            }
            out << "\n";
        }

        file.close();
        return true;

    }

    /// @brief Utilise des scripts python pour exporter au format DOCX / PPTX, return false si le type est différent de ces deux
    /// @return 
    bool exportPython(ExportType type ,const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback)
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
            mediaPath, sar, imageQueue.get(), toImageTimes(items), nullptr,
            std::optional<int>(), std::optional<cv::Size>(imgSize)
        );

        decodeThread->start();

        ImgData imgData{};
        int itemCount = items.size();
        int currItemId = 0;

        while(true) {
            imageQueue->waitPop(imgData);
            if(imgData.isFinished) break;

            if (currItemId < itemCount) {
                // Sauvegarde de l'image
                if (!imgData.img.empty()) {
                    QImage tempImage(imgData.img.data, imgData.img.cols, imgData.img.rows, imgData.img.step, QImage::Format_BGR888);
                    tempImage.save(tempPath + QString("/image_shot_%1.png").arg(currItemId), "PNG");
                }

                if (progressCallback && itemCount > 0) { 
                    int percent = static_cast<int>(((currItemId + 1) * 95.0) / itemCount); // On va de 0 à 95% car c'est le plus long 
                    if (!progressCallback(percent)) {
                        stopDecodeThread(decodeThread, imageQueue.get());
                        return false;
                    }
                }
                ++currItemId;
            }
        }

        stopDecodeThread(decodeThread, imageQueue.get());

        // Preparation json
        QJsonArray jsonShots;
        for (int itemId = 0; itemId < items.size(); ++itemId) {
            QJsonObject shotObj;
            shotObj["id"] = itemId;
            shotObj["title"] = items[itemId].title;
            shotObj["start"] = items[itemId].start;
            shotObj["duration"] = items[itemId].end - items[itemId].start;
            shotObj["imgTxt"] = items[itemId].imgTxt;
            shotObj["soundTxt"] = items[itemId].soundTxt;
            shotObj["image"] = QString("image_shot_%1.png").arg(itemId);
            jsonShots.append(shotObj);
        }

        QJsonObject rootData;
        rootData["fps"] = fps;
        rootData["duration"] = duration;
        rootData["dstPath"] = dstPath;
        rootData["tempDir"] = tempPath;
        rootData["shots"] = jsonShots;
        rootData["mediaName"] = QFileInfo(mediaPath).baseName();

        QJsonDocument doc(rootData);
        QFile jsonFile(tempPath + "/export_data.json");
        if (jsonFile.open(QIODevice::WriteOnly)) {
            jsonFile.write(doc.toJson());
            jsonFile.close();
        }

        QProcess pythonProcess;
        pythonProcess.setProcessChannelMode(QProcess::MergedChannels); // Pour lire les prints normaux et les erreurs

        // On passe le chemin du JSON
        //pythonProcess.start("py", QStringList() << pythonScriptPath << jsonFile.fileName());

        QString appDir = QCoreApplication::applicationDirPath();
        QString pythonExe;

        #if defined(Q_OS_WIN)
            pythonExe = appDir + "/python/python.exe";
        #elif defined(Q_OS_MAC)
            //pythonExe = appDir + "/python/bin/python3";
            pythonExe = appDir + "/../Resources/python/bin/python3";
            //pythonExe = "python3";
        #else
            pythonExe = appDir + "/python/bin/python3";
        #endif

        QString scriptPath = appDir + "/" + pythonScriptPath;
        #if defined(Q_OS_MAC) 
            scriptPath = appDir + "/../Resources/" + pythonScriptPath;
        #endif
        
        qDebug() << "Python script path: " << scriptPath;

        if(!QFile::exists(scriptPath)){
            qDebug() << "Python script not found: " << scriptPath;
            return false;
        }

        QStringList arguments;
        arguments << scriptPath << jsonFile.fileName() << labels.item << labels.startTime << labels.duration
                  << labels.imgTxt << labels.soundTxt;
        pythonProcess.start(pythonExe, arguments);

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


    bool exportVideo(ExportType type, const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback)
    {
        if(progressCallback) progressCallback(0);

        QFileInfo mediaInfo(mediaPath);
        QString extension = mediaInfo.suffix();
        if (type == ExportType::MP4) extension = "mp4";

        QTemporaryDir tempDir;
        if (!tempDir.isValid()) return false;
        QString tempDirPath = tempDir.path();
        QString tempVideo = QDir(tempDirPath).filePath("temp_video." + extension);

        qDebug() << "Chemin du média : " << mediaPath.toStdString();  
        cv::VideoCapture cap;
        bool opened = SLV::openVideoCapture(cap, mediaPath, "Export Video");

        if (!opened) {
            qCritical() << "Export Video: Impossible d'ouvrir la video pour exporter" << mediaPath.toUtf8().constData()
                        << "(exists:" << QFile::exists(mediaPath) << ")";
                        return false;
        } else {
            qDebug() << "Export Video: media opened with path:" << mediaPath;
        }

        int currentFrame = 0;
        int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);

        // On force en mp4 si l'utilisateur veut un export mp4 sinon c'est le même que la source
        int fourcc = (type == ExportType::MP4) ? cv::VideoWriter::fourcc('m', 'p', '4', 'v') : static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));

        cv::Size originalSize(
            static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)),
            static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT))
        );

        cap.release();

        cv::Size displaySize(
            (sar > 0.0) ? static_cast<int>(originalSize.width * sar) : originalSize.width,
            originalSize.height
        );

        cv::VideoWriter writer(
            tempVideo.toLocal8Bit().constData(),
            fourcc,                      
            fps,                         
            displaySize 
        );

        if (!writer.isOpened()) {
            qDebug() << "Le codec n'est pas supporté pour l'écriture. Utilisation de mp4v...";
            
            int fallbackFourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
            writer.open(
                tempVideo.toLocal8Bit().constData(), 
                fallbackFourcc, 
                fps, 
                displaySize 
            );
            bool writerOpened = writer.isOpened();
            if(!writerOpened){
                qDebug() << "Impossible d'écrire en mp4v, écriture en avc1";
                fallbackFourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
                writer.open(
                    tempVideo.toLocal8Bit().constData(), 
                    fallbackFourcc, 
                    fps, 
                    displaySize 
                );
                writerOpened = writer.isOpened();
            }
            if(!writerOpened){
                qDebug() << "Impossible d'écrire en avc1, écriture en mjpg";
                fallbackFourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
                writer.open(
                    tempVideo.toLocal8Bit().constData(), 
                    fallbackFourcc, 
                    fps, 
                    displaySize 
                );
                writerOpened = writer.isOpened();
            }
            if(writerOpened) qDebug() << "écriture Ok";
        }

        int currItemId = -1;
        int64_t endItemItem = -1;

        ImgData imgData;
        QStringList wrappedText;

        int percent = 0;

        auto [fontSize, lineSpacing] = computeFontSizeAndSpacing(displaySize.width, displaySize.height, 0.020);
        QImage textOverlay(displaySize.width, displaySize.height, QImage::Format_ARGB32_Premultiplied);

        std::unique_ptr<TSQueue<ImgData>> imageQueue(new TSQueue<ImgData>(5));
        DecodeThread* decodeThread = new DecodeThread(
            mediaPath, sar, imageQueue.get(), {}
        );

        decodeThread->start();

        while ( true )
        {
            imageQueue->waitPop(imgData);

            if(imgData.isFinished) break;
            if(imgData.img.empty()) continue;

            // Si endItemItem < timeMs => opencv a dépassé la fin du plan, on met à jour le plan courant
            if( endItemItem < imgData.timeMs || currItemId == -1){
                 // Récupère le temps et l'id de l'item comprenant imgData.timeMs
                currItemId = findItemIndexAtTime(items, imgData.timeMs);
                if(currItemId == -1){ // Si pas de temps trouvé, le text devient vide
                    qDebug() << "Impossible de trouver un item qui comprend : " << imgData.timeMs << ", on garde le texte précédent";
                    wrappedText.clear();
                    textOverlay.fill(Qt::transparent); 
                }else { // Le texte est mis à jour avec les infos du nouveau plan
                    auto& s = items[currItemId];
                    endItemItem = s.end;
                    QString shotTitleTxt = "["+ labels.item + " " + QString::number(currItemId+1) + "] " + s.title;
                    QString timecodeTxt = labels.startTime + " : " + TimeFormatter::msToHHMMSSFF(s.start, fps)
                                        + " / "
                                        + labels.duration + " : " + TimeFormatter::msToHHMMSSFF(s.end - s.start, fps);
                    QString noteTxt;
                    if (!s.imgTxt.isEmpty())
                        noteTxt += labels.imgTxt + " : " + s.imgTxt;
                    if (!labels.soundTxt.isEmpty() && !s.soundTxt.isEmpty()) {
                        if (!noteTxt.isEmpty()) noteTxt += "\n";
                        noteTxt += labels.soundTxt + " : " + s.soundTxt;
                    }

                    wrappedText = formatText(shotTitleTxt, timecodeTxt, noteTxt, displaySize.width, fontSize);
                    
                    textOverlay.fill(Qt::transparent); 
                    writeOnOverlay(textOverlay, wrappedText, fontSize, lineSpacing); // Mise à jour de l'overlay à chaque fois que le plan change
                }
            }

            cv::Mat resizedImg;
            if (sar > 0.0 && sar != 1.0) {
                cv::resize(imgData.img, resizedImg, displaySize, 0, 0, cv::INTER_LINEAR);
            } else {
                resizedImg = imgData.img; // Si ratio 1:1, on garde l'image telle quelle
            }

            QImage img(resizedImg.data, resizedImg.cols, resizedImg.rows, static_cast<int>(resizedImg.step), QImage::Format_BGR888);
            
            QPainter painter(&img);
            painter.drawImage(0, 0, textOverlay); // Draw l'image transparente avec le texte par-dessus l'image (rapide)
            
            writer.write(resizedImg);

            if (progressCallback && totalFrames > 0) {
                percent = static_cast<int>(((currentFrame + 1) * 100.0) / totalFrames);
                if(!progressCallback(percent)){
                    stopDecodeThread(decodeThread, imageQueue.get());
                    writer.release();
                    return false;
                }
            }
            
            ++currentFrame;
        }

        stopDecodeThread(decodeThread, imageQueue.get());

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

    std::optional<ExportSelection> selectFormatWindow(const MediaType mediaType, const QString &originalFormat, bool hasAnnotations)
    {
        QDialog dialog;
        auto& txtManager = PrefManager::instance();
        dialog.setWindowTitle(txtManager.getText("export_format_selection_title"));
        dialog.setMinimumWidth(300);

        QVBoxLayout* layout = new QVBoxLayout(&dialog);

        QLabel* sourceLabel = new QLabel(txtManager.getText("export_source_selection_txt"), &dialog);
        layout->addWidget(sourceLabel);

        QRadioButton* shotsRadio = new QRadioButton(txtManager.getText("shots"), &dialog);
        QRadioButton* annotationsRadio = new QRadioButton(txtManager.getText("annotations"), &dialog);
        shotsRadio->setChecked(true);
        annotationsRadio->setEnabled(hasAnnotations);

        QHBoxLayout* sourceLayout = new QHBoxLayout();
        sourceLayout->addWidget(shotsRadio);
        sourceLayout->addWidget(annotationsRadio);
        layout->addLayout(sourceLayout);

        QLabel* label = new QLabel(txtManager.getText("export_format_selection_txt"), &dialog);
        layout->addWidget(label);

        QComboBox* comboBox = new QComboBox(&dialog);
        
        if(mediaType == MediaType::Video){
            comboBox->addItem(txtManager.getText("export_format_txt") + " (.txt)", static_cast<int>(ExportType::TXT));
            comboBox->addItem(txtManager.getText("export_format_pdf") + " (.pdf)", static_cast<int>(ExportType::PDF));
            comboBox->addItem(txtManager.getText("export_format_pptx") + " (.pptx)", static_cast<int>(ExportType::PPTX));
            comboBox->addItem(txtManager.getText("export_format_docx") + " (.docx)", static_cast<int>(ExportType::DOCX));
            comboBox->addItem(txtManager.getText("export_format_csv") + " (.csv)", static_cast<int>(ExportType::CSV));
            if(originalFormat != ".mp4") comboBox->addItem(txtManager.getText("export_format_mp4") + " (.mp4)", static_cast<int>(ExportType::MP4)); // si on est deja en mp4, on n'affiche pas l'option mp4
            comboBox->addItem(txtManager.getText("export_format_src") + " (" + originalFormat + ")", static_cast<int>(ExportType::SRC));
            comboBox->addItem(txtManager.getText("export_format_selection_txt_tagImage"), static_cast<int>(ExportType::TagImage));
        }else {
            comboBox->addItem(txtManager.getText("export_format_txt") + " (.txt)", static_cast<int>(ExportType::TXT));
            comboBox->addItem(txtManager.getText("export_format_csv") + " (.csv)", static_cast<int>(ExportType::CSV));
        }
        
        layout->addWidget(comboBox);

        QObject::connect(annotationsRadio, &QRadioButton::toggled, &dialog, [comboBox, mediaType](bool checked){
            if(mediaType != MediaType::Video) return;

            if(checked) comboBox->removeItem(comboBox->count() - 1) ;
            else {
                auto& txtManager = PrefManager::instance();
                comboBox->addItem(txtManager.getText("export_format_selection_txt_tagImage"), static_cast<int>(ExportType::TagImage));
            }
        });

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        
        buttonBox->button(QDialogButtonBox::Ok)->setText(txtManager.getText("generic_btn_ok"));
        buttonBox->button(QDialogButtonBox::Cancel)->setText(txtManager.getText("generic_dialog_btn_cancel"));
        
        layout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            int selectedExportType = comboBox->currentData().toInt();
            ExportSource source = annotationsRadio->isChecked() ? ExportSource::Annotations : ExportSource::Shots;
            return ExportSelection{static_cast<ExportType>(selectedExportType), source};
        }

        return std::nullopt;
    }



}
