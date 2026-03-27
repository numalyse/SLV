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

/// @brief A partir d'une erreur renvoie le message associé
/// @param error 
/// @return 
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

/// @brief Supprime le projet et son média et envoie un signal pour desactiver l'ui de segmentation 
void ProjectManager::deleteProject() {
    if (m_project) {
        if (m_project->media) {
            m_project->media->deleteLater(); 
        }
        delete m_project;
        m_project = nullptr;
        p_timeline = nullptr;
        setSaveNotNeeded();
        emit projectDeleted();
    }
}

/// @brief Supprime le projet actuel. si 1 seul média reçus, créer un projet. Une fois que les fps et la durée ont été parse, init le projet avec 1 shot
/// @param mediaPaths 
void ProjectManager::requestProjectCreation(const QStringList &mediaPaths) {

    deleteProject();

    if (mediaPaths.size() != 1 || mediaPaths.first().isEmpty()) {
        return;
    }

    m_project = new Project();
    m_project->media = new Media(mediaPaths.first(), this);
    setSaveNotNeeded();
    m_isDurationParsed = false;
    m_isFpsParsed = false;

    connect(m_project->media, &Media::durationParsed, this, [this]() {
        m_isDurationParsed = true;
        this->initProjectShot();
    });

    connect(m_project->media, &Media::fpsParsed, this, [this]() {
        m_isFpsParsed = true;
        this->initProjectShot();
    });
    m_project->media->parse();
}


/// @brief 
/// @param ejectMediaAfterSave Si true, va supprimer le projet + éjecter le média
/// @return 
void ProjectManager::saveProject(bool ejectMediaAfterSave){
    if(!m_project){
        qCritical() << "Trying to save on a null project"; 
        return;
    }

    if( ! m_needSave){ // si pas besoin de save on return et éjecte si besoin
        if(ejectMediaAfterSave){
            discardAndEject();
        }
        return;
    }

    if( ! m_project->path.isEmpty() ){ // si on est deja dans un projet avec un path,
        // on écrit directement dans le json sans copier la vidéo
        writeJson();   
        setSaveNotNeeded();
        if(ejectMediaAfterSave){
            discardAndEject();
        }
        return;
    }

    if( mediaPath().isEmpty() ){
        qCritical() << "Media path du project est vide";
        if(ejectMediaAfterSave){
            discardAndEject();
        }
        return;
    }


    if ( ! createProjectFolder() ){ // on a cliqué sur annulé on return 
        return; 
    }else { // copie du média dans le dossier du projet
        QString destMedia = m_project->path + QDir::separator() + m_project->media->fileName() + '.' + m_project->media->fileExtension();
        copyMedia(m_project->media->filePath(), destMedia, ejectMediaAfterSave);
        setSaveNotNeeded();
        return;
    }

}

QString ProjectManager::mediaPath()
{
    if(m_project){
        if(m_project->media){
            return m_project->media->filePath();
        }
    }
    return "";
}

// slots

/// @brief Une fois que les fps et la durée on été parsed, Créer un project avec un plan de la longueur de la vidéo
void ProjectManager::initProjectShot(){

    if( ! m_isFpsParsed || ! m_isDurationParsed ){
        return;
    }

    Q_ASSERT(m_project);

    if(!m_project){
        qFatal() << "Création d'un plan sur un projet = nullptr ";
        return;
    }

    Shot shot{"Titre", 0, m_project->media->duration()};
    shot.tagImageTime = shot.middle();
    m_project->shots.append(shot);
    
    for (size_t i = 0; i < m_project->shots.size(); i++) {
        qDebug() << m_project->shots[i].title << ", " <<  m_project->shots[i].start << ", " << shot.end;
    }

    qDebug() << "project initialisé";
    emit projectInitialized();
}


bool ProjectManager::createProjectFolder(){
    TextManager& txtManager = TextManager::instance();
    QString fileType = txtManager.get("project_manager_create_project_dialog_file_type") + "(*)";
    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr, 
        tr(txtManager.get("project_manager_create_project_dialog").toStdString().c_str()), 
        QDir::homePath(),
        tr(fileType.toStdString().c_str() )
    );

    if(selectedPath.isEmpty()){
        qDebug() << "Sauvegarde annulée";
        return false; 
    }
    
    QFileInfo fileInfo(selectedPath);

    QDir dir(fileInfo.absolutePath());
    
    if(!dir.exists(fileInfo.baseName())) {
        if(!dir.mkdir(fileInfo.baseName())) {
            qCritical() << "Impossible de créer le dossier";
            return false;
        }
    }

    m_project->path = fileInfo.absolutePath() + QDir::separator() + fileInfo.baseName(); 
    m_project->name = fileInfo.baseName();
    qDebug() << "dossier créé";
    return true;
}

void ProjectManager::deleteFolder(const QString& projectFolderPath) {

    if (projectFolderPath.isEmpty()) {
        qWarning() << "Impossible de supprimer : aucun chemin défini";
        return;
    }

    QDir projectDir(projectFolderPath);

    if (projectDir.exists()) {
        if (projectDir.removeRecursively()) {
            qDebug() << "Dossier du projet supprimé avec succès :" << projectFolderPath;
        } else {
            qCritical() << "Echec de la suppression du dossier :" << projectFolderPath;
        }
    } else {
        qDebug() << "Le dossier n'existe pas :" << projectFolderPath;
    }
}


/// @brief Copie avec un thread dédié un média à l'endroit souhaité.
/// @param sourcePath Path du média a copier
/// @param destPath Path de destination du média copié
/// @param ejectMediaAfterSave Si true, ejecte la vidéo après la copie
/// @return 
bool ProjectManager::copyMedia(const QString& sourcePath, const QString& destPath, bool ejectMediaAfterSave) { 

    if (!QFile::exists(sourcePath)) {
        qCritical() << "Erreur : Le fichier source est introuvable." << sourcePath;
        return false;
    }

    if(m_fileCpyThread){ // suppression ancien thread
        if (m_fileCpyThread->isRunning()) {
            m_fileCpyThread->requestInterruption();
            m_fileCpyThread->wait();
        }
        m_fileCpyThread->deleteLater();
        m_fileCpyThread = nullptr;
    }

    TextManager& txtManager = TextManager::instance();

    m_fileCpyThread = new FileCopyThread(sourcePath, destPath, this);
    
    QProgressDialog* progressDialog = new QProgressDialog(txtManager.get("project_window_title_copy_video"), txtManager.get("generic_dialog_btn_cancel"), 0, 100, nullptr);
    progressDialog->setWindowTitle(txtManager.get("project_window_title_copy_video"));
    progressDialog->setWindowModality(Qt::WindowModal); 
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    progressDialog->show();


    connect(progressDialog, &QProgressDialog::canceled, this, [this](){ 
        m_fileCpyThread->requestInterruption();
    });

    connect(m_fileCpyThread, &FileCopyThread::progress, progressDialog, &QProgressDialog::setValue);

    connect(m_fileCpyThread, &FileCopyThread::copyFinished, this, [this, ejectMediaAfterSave, progressDialog, destPath, txtManager](bool success) {
        
        if (success) {
            writeJson();
        }else {
            if ( ! m_fileCpyThread->isInterruptionRequested()) { // si c'est pas un fail demandé par l'utilisateur
                QMessageBox::critical(nullptr, txtManager.get("dialog_error_text"), txtManager.get("project_error_copy_failed"));
            }
            deleteFolder(destPath);
        }

        if(ejectMediaAfterSave){
            discardAndEject();
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

    QString jsonPath = m_project->path + QDir::separator() + m_project->name + ".json";

    std::ofstream projectData(jsonPath.toStdString());
    if ( !projectData.is_open() ) {
        qCritical() << "Impossible d'ouvrir le fichier";
        return false;
    }

    nlohmann::json j; 

    j["media"] = writeMediaData();

    j["shots"] = writeShotsData();

    projectData << j.dump(4);

    projectData.close();

    return true;
}


/// @brief Parcours le JSON pour récupérer les données du json
/// @param projectAbsolutePath Path du projet
/// @return Retourne une structure avec les données du project ou une enum en fonction de l'erreur rencontrées
std::expected<ProjectSaveData, ProjectManager::Error> ProjectManager::loadProject(const QString &projectAbsolutePath)
{
    QFileInfo projectInfo(projectAbsolutePath);

    if (!projectInfo.exists() || !projectInfo.isDir()) {
        qCritical() << "Erreur : Le dossier du projet n'existe pas.";
        return std::unexpected(ProjectManager::Error::FolderNotFound);
    }

    QString projectName = projectInfo.baseName(); 

    QString jsonFilePath = projectAbsolutePath + QDir::separator() + projectName + ".json";

    QFileInfo saveFileInfo(jsonFilePath);
    if (!saveFileInfo.exists() || !saveFileInfo.isFile()) {
        qCritical() << "Erreur : Fichier de projet introuvable au chemin : " << jsonFilePath;
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
                qCritical() << "Erreur : Le fichier vidéo n'est pas dans le dossier ";
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
        qCritical() << "Erreur de format JSON :" << e.what();
        return std::unexpected(ProjectManager::Error::JsonParsingError);
    } 
    catch (const std::exception& e) {
        return std::unexpected(ProjectManager::Error::UnexpectedError);
    }

}

/// @brief Ouvre un dialogue pour choisir l'emplacement du dossier. Parcours le JSON, affiche une erreur dans une message box s'il y en eu, créer projet sinon.
/// Puis parse le media
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
        QMessageBox::critical(nullptr, TextManager::instance().get("dialog_error_text"), errorMsg);
        return;
    }

    ProjectSaveData projectData = loaded.value();

    Project* project = new Project{
        projectData.shots, 
        new Media(projectData.mediaAbsolutePath, this), 
        QFileInfo(selectedPath).baseName(), 
        selectedPath
    };

    deleteProject();

    m_project = project;

    setSaveNotNeeded();
    m_isDurationParsed = false;
    m_isFpsParsed = false;

    auto& txtManager = TextManager::instance();

    connect(m_project->media, &Media::durationParsed, this, [this, txtManager, durationJson = projectData.duration ](int64_t durationFile) {
        
        if(durationJson == durationFile){
            m_isDurationParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchDuration);
            QMessageBox::critical(nullptr, txtManager.get("dialog_error_text"), errorMsg);
        }

    });

    connect(m_project->media, &Media::fpsParsed, this, [this, txtManager, fpsJson = projectData.fps](double fpsFile) {
        
        if(fpsJson == fpsFile){
            m_isFpsParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchFPS);
            QMessageBox::critical(nullptr, txtManager.get("dialog_error_text"), errorMsg);
        }

    });
    
    m_project->media->parse();

}

///@brief Quand les fps et la duréer sont retrouvés, lance un signal pour créer un layout avec 1 player et lance un signal pour créer la timeline
void ProjectManager::checkMediaFullyLoaded()
{
    if(m_isDurationParsed && m_isFpsParsed){

        const QStringList paths {m_project->media->filePath()};

        emit loadMediaProjectRequested(paths);
        emit projectInitialized();
    }
}

void ProjectManager::discardAndEject(){
    deleteProject();
    emit ejectMedia();
}

void ProjectManager::setSaveNeeded(){
    m_needSave = true;
    emit enableSaveButton();
}

void ProjectManager::setSaveNotNeeded(){
    m_needSave = false;
    emit disableSaveButton();
}