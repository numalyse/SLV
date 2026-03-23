#include "ProjectManager.h"

#include "Project.h"
#include "Media.h"
#include "TextManager.h"
#include "External/nlohmann/json.hpp"

#include <QString>
#include <QDebug>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFile>
#include <QProgressDialog>
#include <QDebug>
#include <QMessageBox>

#include <fstream>
#include <iostream>


ProjectManager::ProjectManager(QObject* parent) : QObject(parent)
{
}

ProjectManager::~ProjectManager()
{
    if(m_project){
        delete m_project;
        m_project = nullptr;
    }
}


QString ProjectManager::getErrorMessage(Error error) const
{
    TextManager& txtManager = TextManager::instance();

    switch (error) {
        case Error::FolderNotFound:
            return txtManager.get("project_manager_error_folder_not_found"); 
        case Error::JsonFileNotFound:
            return txtManager.get("project_manager_error_json_file_not_found");   
        case Error::CannotOpenJsonFile:
            return txtManager.get("project_manager_error_cannot_open_json_file");
        case Error::JsonParsingError:
            return txtManager.get("project_manager_error_json_parsing_error");
        case Error::MediaFileNotFound:
            return txtManager.get("project_manager_error_media_file_not_found"); 
        case Error::MediaKeyMissing:
            return txtManager.get("project_manager_error_media_key_missing"); 
        case Error::MismatchFPS:
            return txtManager.get("project_manager_error_media_mismatch_fps"); 
        case Error::MismatchDuration:
            return txtManager.get("project_manager_error_media_mismatch_duration"); 
        case Error::UnexpectedError:
        default:
            return txtManager.get("project_manager_error_unexpected_error");
    }
}


void ProjectManager::createProject(Media* media) 
{   
    if(m_project){
        delete m_project;
        m_project = nullptr;
    }
    
    m_project = new Project();
    m_project->media = new Media(media->filePath(), this);
    connect(m_project->media, &Media::durationParsed, this, &ProjectManager::initProjectShot);
    m_project->media->parse();
}



bool ProjectManager::saveProject(bool ejectMediaAfterSave){

    if( m_project->media->filePath().isEmpty() ){
        qDebug() << "Media path du project est vide";
        if (ejectMediaAfterSave){
            emit ejectMedia();
        }
        
        return false;
    }

    if(m_project->path.isEmpty()){
        if ( ! createProjectFolder()){
            if (ejectMediaAfterSave){
                emit ejectMedia();
            }
            return false;
        }
    }

    QString destMedia = m_project->path + QDir::separator() + m_project->name + QDir::separator()  + m_project->media->fileName() + '.' + m_project->media->fileExtension();
    copyMedia(m_project->media->filePath(), destMedia, ejectMediaAfterSave);

    return true;
}


// slots

/// @brief Créer un plan de la durée de la vidéo quand media a fini de parse pour la durée.
/// @param mediaDuration 
void ProjectManager::initProjectShot(int64_t mediaDuration){

    if(!m_project){
        qDebug() << "project nullptr";
        return;
    }

    Shot shot{"Titre", 0, mediaDuration};
    m_project->shots.append(shot);
    
    for (size_t i = 0; i < m_project->shots.size(); i++) {
        qDebug() << m_project->shots[i].title << ", " <<  m_project->shots[i].start << ", " << shot.end;
    }

    qDebug() << "project initialisé";
    emit projectInitialized();
}


bool ProjectManager::createProjectFolder(){
    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr, 
        tr(TextManager::instance().get("project_manager_create_project_dialog").toStdString().c_str()), 
        QDir::homePath(),
        tr("Projet (*)") // TODO text Manager
    );

    if(selectedPath.isEmpty()){
        qDebug() << "Sauvegarde annulée";
        return false; 
    }
    
    QFileInfo fileInfo(selectedPath);

    QDir dir(fileInfo.absolutePath());
    
    if(!dir.exists(fileInfo.baseName())) {
        if(!dir.mkdir(fileInfo.baseName())) {
            qDebug() << "Impossible de créer le dossier.";
            return false;
        }
    }

    m_project->path = fileInfo.absolutePath(); 
    m_project->name = fileInfo.baseName();
    qDebug() << "dossier créé";
    return true;
}



bool ProjectManager::copyMedia(const QString& sourcePath, const QString& destPath, bool ejectMediaAfterSave) { 

    if (!QFile::exists(sourcePath)) {
        qDebug() << "Erreur : Le fichier source est introuvable." << sourcePath;
        return false;
    }

    if(m_fileCpyThread){
        if (m_fileCpyThread->isRunning()) {
            m_fileCpyThread->requestInterruption();
            m_fileCpyThread->wait();
        }
        m_fileCpyThread->deleteLater();
        m_fileCpyThread = nullptr;
    }

    TextManager& txtManger = TextManager::instance();

    m_fileCpyThread = new FileCopyThread(sourcePath, destPath, this);
    
    QProgressDialog* progressDialog = new QProgressDialog(txtManger.get("project_copy_video"), txtManger.get("cancel"), 0, 100, nullptr);
    progressDialog->setWindowTitle(txtManger.get("project_window_title_copy_video"));
    progressDialog->setWindowModality(Qt::WindowModal); 
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    progressDialog->show();


    connect(progressDialog, &QProgressDialog::canceled, this, [this](){
        m_fileCpyThread->requestInterruption();
    });

    connect(m_fileCpyThread, &FileCopyThread::progress, progressDialog, &QProgressDialog::setValue);

    connect(m_fileCpyThread, &FileCopyThread::copyFinished, this, [this, ejectMediaAfterSave, progressDialog](bool success) {
        if (success) {
            // creation json
            writeJson();
        }
        if(ejectMediaAfterSave){
            delete m_project;
            m_project = nullptr;
            p_timeline = nullptr;
            m_askSave = false;
            emit ejectMedia();
            emit projectDeleted();
        }
        progressDialog->close(); 
    });

    m_fileCpyThread->start();

    return true;
}


nlohmann::json ProjectManager::writeShotsData(){
    Q_ASSERT( p_timeline );

    auto shots = p_timeline->getTimelineData();

    Q_ASSERT( ! shots.empty() );

    nlohmann::json shotArray = nlohmann::json::array();

    for( int IShot = 0; IShot < shots.size(); ++IShot ){

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

nlohmann::json ProjectManager::writeMediaData(){
    Q_ASSERT( p_timeline );

    auto shots = p_timeline->getTimelineData();

    Q_ASSERT( ! shots.empty() );

    nlohmann::json mediaData = nlohmann::json::object();

    auto projectMedia = m_project->media;

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

bool ProjectManager::writeJson()
{


    qDebug() << "Writting Json";

    QString jsonPath = m_project->path + QDir::separator() + m_project->name + QDir::separator() + m_project->name + ".json";

    std::ofstream projectData(jsonPath.toStdString());
    if ( !projectData.is_open() ) {
        qDebug() << "Impossible d'ouvrir le fichier";
        return false;
    }

    nlohmann::json j; 

    j["media"] = writeMediaData();

    j["shots"] = writeShotsData();

    projectData << j.dump(4);

    projectData.close();

    return true;
}



std::expected<ProjectSaveData, ProjectManager::Error> ProjectManager::loadProject(const QString &projectAbsolutePath)
{
    QFileInfo projectInfo(projectAbsolutePath);

    if (!projectInfo.exists() || !projectInfo.isDir()) {
        qDebug() << "Erreur : Le dossier du projet n'existe pas.";
        return std::unexpected(ProjectManager::Error::FolderNotFound);
    }

    QString projectName = projectInfo.baseName(); 

    QString jsonFilePath = projectAbsolutePath + QDir::separator() + projectName + ".json";

    QFileInfo saveFileInfo(jsonFilePath);
    if (!saveFileInfo.exists() || !saveFileInfo.isFile()) {
        qDebug() << "Erreur : Fichier de projet introuvable au chemin : " << jsonFilePath;
        return std::unexpected(ProjectManager::Error::JsonFileNotFound);
    }

    try {
        std::ifstream file(jsonFilePath.toLocal8Bit().constData());
        if (!file.is_open()) {
            return std::unexpected(ProjectManager::Error::CannotOpenJsonFile);
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
                qDebug() << "Erreur : Le fichier vidéo n'est pas dans le dossier ";
                return std::unexpected(ProjectManager::Error::MediaFileNotFound);
            }

        } else {
            return std::unexpected(ProjectManager::Error::MediaKeyMissing);
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
            return std::unexpected(ProjectManager::Error::MediaKeyMissing);
        }

        return loadedData;

    } 
    // Capture les erreurs spécifiques au format JSON 
    catch (const nlohmann::json::exception& e) {
        qDebug() << "Erreur de format JSON :" << e.what();
        return std::unexpected(ProjectManager::Error::JsonParsingError);
    } 
    catch (const std::exception& e) {
        return std::unexpected(ProjectManager::Error::UnexpectedError);
    }

}

void ProjectManager::openProject()
{

    QString selectedPath = QFileDialog::getExistingDirectory(
        nullptr, 
        tr(TextManager::instance().get("project_manager_open_project_dialog").toStdString().c_str()), 
        QDir::homePath()
    );

    if(selectedPath.isEmpty()){
        return;
    }

    auto loaded = loadProject(selectedPath);

    if (!loaded.has_value()) {
        QString errorMsg = getErrorMessage(loaded.error());
        QMessageBox::critical(nullptr, "Erreur", errorMsg);
    }

    ProjectSaveData projectData = loaded.value();

    Project* project = new Project{
        projectData.shots, 
        new Media(projectData.mediaAbsolutePath, this), 
        QFileInfo(selectedPath).baseName(), 
        selectedPath
    };

    if(m_project){
        delete m_project;
        m_project = nullptr;
    }

    m_project = project;

    m_isDurationParsed = false;
    m_isFpsParsed = false;

    connect(m_project->media, &Media::durationParsed, this, [this, durationJson = projectData.duration ](int64_t durationFile) {
        
        if(durationJson == durationFile){
            m_isDurationParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchDuration);
            QMessageBox::critical(nullptr, "Erreur", errorMsg);
        }

    });

    connect(m_project->media, &Media::fpsParsed, this, [this, fpsJson = projectData.fps](double fpsFile) {
        
        if(fpsJson == fpsFile){
            m_isFpsParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchFPS);
            QMessageBox::critical(nullptr, "Erreur", errorMsg);
        }

    });
    
    m_project->media->parse();

}

void ProjectManager::checkMediaFullyLoaded()
{
    if(m_isDurationParsed && m_isFpsParsed){

        const QStringList paths {m_project->media->filePath()};

        emit loadMediaProjectRequested(paths);
        emit projectInitialized();
    }
}

