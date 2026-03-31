#include "Project/ProjectFileHandler.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <fstream>

#include <External/nlohmann/json.hpp>

namespace  {
    
    nlohmann::json writeShotsData(TimelineWidget* timeline) {
        auto shots = timeline->getTimelineData();
        nlohmann::json shotArray = nlohmann::json::array();

        for(int IShot = 0; IShot < shots.size(); ++IShot) {
            nlohmann::json shotObject;
            shotObject["title"] = shots[IShot].title.toStdString();
            shotObject["start"] = shots[IShot].start;
            shotObject["end"] = shots[IShot].end;
            shotObject["tagImageTime"] = shots[IShot].tagImageTime;
            shotObject["note"] = shots[IShot].note.toStdString();
            shotArray.push_back(shotObject);
        }
        return shotArray;
    }

    nlohmann::json writeMediaData(const Project* project) {
        nlohmann::json mediaData = nlohmann::json::object();
        auto projectMedia = project->media;

        mediaData["filePath"] = projectMedia->filePath().toStdString();
        QString filename = projectMedia->fileName() + "." + projectMedia->fileExtension();
        mediaData["name"] = filename.toStdString();
        mediaData["duration"] = projectMedia->duration();
        mediaData["fps"] = projectMedia->fps();
        mediaData["type"] = projectMedia->type();

        nlohmann::json metaDataObject = nlohmann::json::object();
        auto meta = projectMedia->metaData();
        QMapIterator<libvlc_meta_t, QString> IMeta(meta);
        while (IMeta.hasNext()) {
            IMeta.next();
            int idInt = static_cast<int>(IMeta.key());
            std::string idString = std::to_string(idInt);
            metaDataObject[idString] = IMeta.value().toStdString();
        }
        mediaData["metaData"] = metaDataObject;

        return mediaData;
    }
}

namespace ProjectFileHandler {

    std::expected<ProjectSaveData, ProjectFileError> ProjectFileHandler::loadProject(const QString& projectAbsolutePath) {
        QFileInfo projectInfo(projectAbsolutePath);

        if (!projectInfo.exists() || !projectInfo.isDir()) {
            qCritical() << "Erreur : Le dossier du projet n'existe pas.";
            return std::unexpected(ProjectFileError::FolderNotFound);
        }

        QString projectName = projectInfo.baseName(); 

        QString jsonFilePath = projectAbsolutePath + QDir::separator() + projectName + ".json";

        QFileInfo saveFileInfo(jsonFilePath);
        if (!saveFileInfo.exists() || !saveFileInfo.isFile()) {
            qCritical() << "Erreur : Fichier de projet introuvable au chemin : " << jsonFilePath;
            return std::unexpected(ProjectFileError::JsonFileNotFound);
        }

        try {
            std::ifstream file(jsonFilePath.toLocal8Bit().constData());
            if (!file.is_open()) {
                return std::unexpected(ProjectFileError::CannotOpenJsonFile);
            }

            nlohmann::json projectData;
            file >> projectData; 
            file.close();

            ProjectSaveData loadedData;

            if (projectData.contains("media")) {
                auto mediaJson = projectData["media"];

                std::string nameStr = mediaJson.value("name", "");
                loadedData.mediaName = QString::fromStdString(nameStr);
                loadedData.duration = mediaJson.value("duration", 0);
                loadedData.fps = mediaJson.value("fps", 0.0);

                loadedData.mediaAbsolutePath = projectAbsolutePath + QDir::separator() + loadedData.mediaName;
                QFileInfo mediaInfo(loadedData.mediaAbsolutePath);
                
                if (!mediaInfo.exists() || !mediaInfo.isFile()) {
                    qCritical() << "Erreur : Le fichier vidéo n'est pas dans le dossier ";
                    return std::unexpected(ProjectFileError::MediaFileNotFound);
                }

            } else {
                return std::unexpected(ProjectFileError::MediaKeyMissing);
            }

            if(projectData.contains("shots") && projectData["shots"].is_array()){
                for (const auto& shotJson : projectData["shots"]) {
                    Shot currentShot; 
                    
                    currentShot.title = QString::fromStdString(shotJson.value("title", ""));
                    currentShot.start = shotJson.value("start", 0LL); 
                    currentShot.end = shotJson.value("end", 0LL);
                    currentShot.tagImageTime = shotJson.value("tagImageTime", 0LL);
                    currentShot.note = QString::fromStdString(shotJson.value("note", ""));

                    loadedData.shots.append(currentShot);
                }
            }else {
                return std::unexpected(ProjectFileError::MediaKeyMissing);
            }

            return loadedData;
        } 
        // Capture les erreurs spécifiques au format JSON 
        catch (const nlohmann::json::exception& e) {
            qCritical() << "Erreur de format JSON :" << e.what();
            return std::unexpected(ProjectFileError::JsonParsingError);
        } 
        catch (const std::exception& e) {
            return std::unexpected(ProjectFileError::UnexpectedError);
        }
    }


    bool writeJson(const Project* project, TimelineWidget* timeline) {
        if (!project || !timeline) return false;

        QString jsonPath = project->path + QDir::separator() + project->name + ".json";

        QFile projectData(jsonPath);
        if (!projectData.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "Impossible d'ouvrir le fichier en écriture :" << jsonPath;
            return false;
        }

        nlohmann::json j;
        j["media"] = writeMediaData(project);
        j["shots"] = writeShotsData(timeline);

        std::string jsonString = j.dump(4);
        projectData.write(jsonString.c_str(), jsonString.size());
        projectData.close();

        return true;
    }

}