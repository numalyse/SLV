#include "Project/ProjectFileHelper.h"

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

    QJsonObject writeMediaData(const Project* project) {
        QJsonObject mediaData;
        auto projectMedia = project->media;

        mediaData["filePath"] = projectMedia->filePath();
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

        if (!projectInfo.exists() || !projectInfo.isDir()) {
            qCritical() << "Erreur : Le dossier du projet n'existe pas.";
            return std::unexpected(ProjectFileError::FolderNotFound);
        }

        QString projectName = projectInfo.baseName(); 
        QString jsonFilePath = QDir(projectAbsolutePath).filePath(projectName + ".json");
        
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

            loadedData.mediaName = mediaJson.value("name").toString("");
            loadedData.duration = mediaJson.value("duration").toInt(0);
            loadedData.fps = mediaJson.value("fps").toDouble(0.0);

            loadedData.mediaAbsolutePath = QDir(projectAbsolutePath).filePath(loadedData.mediaName);
            QFileInfo mediaInfo(loadedData.mediaAbsolutePath);
            
            if (!mediaInfo.exists() || !mediaInfo.isFile()) {
                qCritical() << "Erreur : Le fichier vidéo n'est pas dans le dossier ";
                return std::unexpected(ProjectFileError::MediaFileNotFound);
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

        return loadedData;
    }


    bool writeJson(const Project* project, TimelineWidget* timeline) {
        if (!project || !timeline) return false;

        QString jsonPath = QDir(project->path).filePath(project->name + ".json");
        
        QFile projectDataFile(jsonPath);
        if (!projectDataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "Impossible d'ouvrir le fichier en écriture :" << jsonPath;
            return false;
        }

        QJsonObject j;
        j["media"] = writeMediaData(project);
        j["shots"] = writeShotsData(timeline);

        QJsonDocument doc(j);
        projectDataFile.write(doc.toJson(QJsonDocument::Indented)); 
        projectDataFile.close();

        return true;
    }

}