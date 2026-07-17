#include "Project/ProjectFileHelper.h"
#include "MacSymLink.h"
#include "WinSymLink.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace  {
    
    QJsonArray writeShotsData(TimelineWidget* timeline) {
        auto shots = timeline->getTimelineData();
        QJsonArray shotArray;

        for(const auto& shot : shots) {
            QJsonObject shotObject;
            shotObject["title"] = shot.title;
            shotObject["start"] = shot.start;
            shotObject["end"] = shot.end;
            shotObject["tagImageTime"] = shot.tagImageTime;
            shotObject["note"] = shot.note;
            shotArray.append(shotObject);
        }
        return shotArray;
    }

    QJsonArray writeAnnotsData(const Project* project) {
        const auto& annotations = project->annotations;
        QJsonArray annotArray;

        for(const auto& annotation : annotations) {
            QJsonObject annotObject;
            annotObject["id"] = annotation.id;
            annotObject["start"] = annotation.start;
            annotObject["end"] = annotation.end;
            annotObject["name"] = annotation.name;
            annotObject["note"] = annotation.note;
            annotObject["color"] = annotation.color.name();
            annotArray.append(annotObject);
        }
        return annotArray;
    }

    QJsonObject writeMediaData(const Project* project) {
        QJsonObject mediaData;
        auto projectMedia = project->media;

        mediaData["mediaLinkPath"] = project->mediaLinkPath;
        QString filename = projectMedia->fileName() + "." + projectMedia->fileExtension();
        mediaData["name"] = filename;
        mediaData["duration"] = projectMedia->duration();
        mediaData["fps"] = projectMedia->fps();
        mediaData["type"] =  QString::number(static_cast<int>(projectMedia->type()));

        QJsonObject metaDataObject;
        auto meta = projectMedia->metaData();
        QMapIterator<libvlc_meta_t, QString> IMeta(meta);
        while (IMeta.hasNext()) {
            IMeta.next();
            QString idString = QString::number(static_cast<int>(IMeta.key()));
            metaDataObject[idString] = IMeta.value();
        }
        mediaData["metaData"] = metaDataObject;

        return mediaData;
    }
}

namespace ProjectFileHelper {

    std::expected<ProjectSaveData, ProjectFileError> ProjectFileHelper::loadProject(const QString& projectAbsolutePath) {
        QFileInfo projectInfo(projectAbsolutePath);
        QDir projDir = QDir(projectAbsolutePath);
        qDebug() << "QFILEInfo Loading project from path:" << projectAbsolutePath;

        if (!projectInfo.exists() || !projectInfo.isDir()) {
            qCritical() << "Erreur : Le dossier du projet n'existe pas.";
            return std::unexpected(ProjectFileError::FolderNotFound);
        }

        QString projectName = projDir.dirName(); 
        qDebug() << "Project Name with baseName: " << projectName;

        QString jsonFilePath = QDir(projectAbsolutePath).filePath(projectName + ".json");
        qDebug() << "Loading project from JSON file path:" << jsonFilePath;
    

        QFileInfo saveFileInfo(jsonFilePath);
        if (!saveFileInfo.exists() || !saveFileInfo.isFile()) {
            qCritical() << "Erreur : Fichier de projet introuvable au chemin : " << jsonFilePath;
            return std::unexpected(ProjectFileError::JsonFileNotFound);
        }

        QFile file(jsonFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return std::unexpected(ProjectFileError::CannotOpenJsonFile);
        }

        QByteArray saveData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(saveData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qCritical() << "Erreur de format JSON :" << parseError.errorString();
            return std::unexpected(ProjectFileError::JsonParsingError);
        }

        if (!jsonDoc.isObject()) {
            return std::unexpected(ProjectFileError::JsonParsingError);
        }

        QJsonObject projectData = jsonDoc.object();
        ProjectSaveData loadedData;

        if (projectData.contains("media") && projectData["media"].isObject()) {
            QJsonObject mediaJson = projectData["media"].toObject();

            loadedData.mediaLinkAbsolutePath = mediaJson.value("mediaLinkPath").toString("");
            loadedData.duration = mediaJson.value("duration").toInt(0);
            loadedData.fps = mediaJson.value("fps").toDouble(0.0);

#ifdef Q_OS_MACOS
            loadedData.mediaAbsolutePath = MacSymLink::findTarget(loadedData.mediaLinkAbsolutePath);
#elif defined(Q_OS_WIN)
            // QFile::symLinkTarget only reads the path stored in the .lnk;
            // WinSymLink calls IShellLink::Resolve so windows finds the target even if it moved
            loadedData.mediaAbsolutePath = WinSymLink::findTarget(loadedData.mediaLinkAbsolutePath);
#else
            loadedData.mediaAbsolutePath = QFile::symLinkTarget(loadedData.mediaLinkAbsolutePath);
#endif

            QFileInfo mediaInfo(loadedData.mediaAbsolutePath);

            if ( loadedData.mediaAbsolutePath == "" || !mediaInfo.exists() || !mediaInfo.isFile()) {
                // the link or its target is gone : leave the media path empty,
                // the caller asks the user to locate the media again
                qWarning() << "[ProjectFileHelper] load project : failed to retrieve the medialink target";
                loadedData.mediaAbsolutePath.clear();
            } else {
                loadedData.mediaName = mediaInfo.fileName();
            }

        } else {
            return std::unexpected(ProjectFileError::MediaKeyMissing);
        }

        if(projectData.contains("shots") && projectData["shots"].isArray()){
            QJsonArray shotsArray = projectData["shots"].toArray();
            for (const QJsonValue& value : shotsArray) {
                QJsonObject shotJson = value.toObject();
                Shot currentShot; 
                
                currentShot.title = shotJson.value("title").toString("");
                currentShot.start = shotJson.value("start").toInteger(0LL); 
                currentShot.end = shotJson.value("end").toInteger(0LL);
                currentShot.tagImageTime = shotJson.value("tagImageTime").toInteger(0LL);
                currentShot.note = shotJson.value("note").toString("");

                loadedData.shots.append(currentShot);
            }
        } else {
            return std::unexpected(ProjectFileError::MediaKeyMissing); 
        }

        if(projectData.contains("annots") && projectData["annots"].isArray()){
            QJsonArray annotsArray = projectData["annots"].toArray();
            for (const QJsonValue& value : annotsArray) {
                QJsonObject annotJsonObj = value.toObject();
                Annotation annot; 
                
                annot.id = annotJsonObj.value("id").toInteger();
                annot.start = annotJsonObj.value("start").toInteger(0LL); 
                annot.end = annotJsonObj.value("end").toInteger(0LL);
                annot.name = annotJsonObj.value("name").toString("");
                annot.note = annotJsonObj.value("note").toString("");
                QColor color = QColor::fromString(annotJsonObj.value("color").toString(""));
                if (color.isValid()) {
                    annot.color = color;
                }

                loadedData.annots.append(annot);
            }
        }else {
            loadedData.annots = {};
        }

        return loadedData;
    }


    bool createMediaLink(const QString& mediaAbsolutePath, const QString& linkAbsolutePath) {
        QFile::remove(linkAbsolutePath);
#ifdef Q_OS_MACOS
        return MacSymLink::create(mediaAbsolutePath, linkAbsolutePath);
#else
        return QFile::link(mediaAbsolutePath, linkAbsolutePath);
#endif
    }


    bool writeJson(const Project* project, TimelineWidget* timeline) {
        if (!project || !timeline) return false;

        QDir projDir = QDir(project->path);
        QString jsonPath = projDir.filePath(projDir.dirName() + ".json");
        
        QFile projectDataFile(jsonPath);
        if (!projectDataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "Impossible d'ouvrir le fichier en écriture :" << jsonPath;
            return false;
        }

        QJsonObject j;
        j["media"] = writeMediaData(project);
        j["shots"] = writeShotsData(timeline);
        j["annots"] = writeAnnotsData(project);

        QJsonDocument doc(j);
        projectDataFile.write(doc.toJson(QJsonDocument::Indented)); 
        projectDataFile.close();

        return true;
    }

}