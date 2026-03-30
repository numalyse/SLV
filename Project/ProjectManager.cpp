#include "Project/ProjectManager.h"

#include "Project/Project.h"
#include "Media.h"
#include "TextManager.h"
#include "External/nlohmann/json.hpp"
#include "Project/ProjectFileHandler.h"
#include "Project/ProjectExportThread.h"
#include "FileCopyThread.h"

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
QString ProjectManager::getErrorMessage(ProjectFileError error) const
{
    TextManager& txtManager = TextManager::instance();
    switch (error) {
        case ProjectFileError::FolderNotFound:
            return txtManager.get("project_manager_error_folder_not_found"); 
        case ProjectFileError::JsonFileNotFound:
            return txtManager.get("project_manager_error_json_file_not_found");   
        case ProjectFileError::CannotOpenJsonFile:
            return txtManager.get("project_manager_error_cannot_open_json_file");
        case ProjectFileError::JsonParsingError:
            return txtManager.get("project_manager_error_json_parsing_error");
        case ProjectFileError::MediaFileNotFound:
            return txtManager.get("project_manager_error_media_file_not_found"); 
        case ProjectFileError::MediaKeyMissing:
            return txtManager.get("project_manager_error_media_key_missing"); 

        case ProjectFileError::UnexpectedError:
        default:
            return txtManager.get("project_manager_error_unexpected_error");
    }
}

QString ProjectManager::getErrorMessage(ProjectManager::Error error) const {
    TextManager& txtManager = TextManager::instance();
    switch (error) {
        case ProjectManager::Error::MismatchFPS:
            return txtManager.get("project_manager_error_media_mismatch_fps"); 
        case ProjectManager::Error::MismatchDuration:
            return txtManager.get("project_manager_error_media_mismatch_duration"); 
        case ProjectManager::Error::UnexpectedError:
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
        ProjectFileHandler::writeJson(m_project, p_timeline);
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

QString ProjectManager::mediaPathExtension()
{
    if(m_project){
        if(m_project->media){
            return m_project->media->fileExtension();
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

    TextManager& txtManager = TextManager::instance();

    FileCopyThread* fileCpyThread = new FileCopyThread(sourcePath, destPath, this);
    
    QProgressDialog* progressDialog = new QProgressDialog(txtManager.get("project_window_title_copy_video"), txtManager.get("generic_dialog_btn_cancel"), 0, 100, nullptr);
    progressDialog->setWindowTitle(txtManager.get("project_window_title_copy_video"));
    progressDialog->setWindowModality(Qt::WindowModal); 
    progressDialog->show();


    connect(progressDialog, &QProgressDialog::canceled, this, [fileCpyThread](){ 
        fileCpyThread->requestInterruption();
    });

    connect(fileCpyThread, &FileCopyThread::progress, progressDialog, &QProgressDialog::setValue);

    connect(fileCpyThread, &FileCopyThread::copyFinished, this, [this, fileCpyThread, ejectMediaAfterSave, progressDialog, destPath](bool success, bool canceled) {
        
        if (success) {
            ProjectFileHandler::writeJson(m_project, p_timeline);
        } else {
            if ( !canceled ) { // si c'est pas un fail demandé par l'utilisateur
                auto& txtManager = TextManager::instance(); 
                QMessageBox::critical(nullptr, txtManager.get("dialog_error_text"), txtManager.get("project_error_copy_failed"));
            }
            deleteFolder(destPath);
        }

        if(ejectMediaAfterSave){
            discardAndEject();
        }

        progressDialog->close(); 
        progressDialog->deleteLater(); 
        fileCpyThread->deleteLater(); 
    });

    fileCpyThread->start();

    return true;
}



/// @brief Ouvre un dialogue pour choisir l'emplacement du dossier. Parcours le JSON, affiche une erreur dans une message box s'il y en eu, créer projet sinon.
/// Puis parse le media
void ProjectManager::openProject()
{
    auto& txtManager = TextManager::instance();

    QString selectedPath = QFileDialog::getExistingDirectory(
        nullptr, 
        tr(txtManager.get("project_manager_open_project_dialog").toStdString().c_str()), 
        QDir::homePath()
    );

    if(selectedPath.isEmpty()){
        return;
    }

    auto loaded = ProjectFileHandler::loadProject(selectedPath);

    if (!loaded.has_value()) {
        QString errorMsg = getErrorMessage(loaded.error());
        QMessageBox::critical(nullptr, txtManager.get("dialog_error_text"), errorMsg);
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


    connect(m_project->media, &Media::durationParsed, this, [this, durationJson = projectData.duration ](int64_t durationFile) {
        
        if(durationJson == durationFile){
            m_isDurationParsed = true;
            checkMediaFullyLoaded();
        } else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchDuration);
            QMessageBox::critical(nullptr, TextManager::instance().get("dialog_error_text"), errorMsg);
        }

    });

    connect(m_project->media, &Media::fpsParsed, this, [this, fpsJson = projectData.fps](double fpsFile) {
        
        if(fpsJson == fpsFile){
            m_isFpsParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchFPS);
            QMessageBox::critical(nullptr, TextManager::instance().get("dialog_error_text"), errorMsg);
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

void ProjectManager::exportProject(){
    if ( !p_timeline ) return;
    if ( !m_project ) return;
    if ( !m_project->media ) return;

    TextManager& txtManager = TextManager::instance();

    QString extension = '.' + mediaPathExtension();

    if ( extension.isEmpty() ) return;

    auto format = ProjectExportHandler::selectFormatWindow(extension);

    if ( ! format.has_value() ) return;
    
    ExportType selectedFormat = format.value();

    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr, 
        txtManager.get("export_file_path_title"), 
        m_project->path, 
        txtManager.get("export_file_path_file_format") 
    );

    if ( selectedPath.isEmpty() ) return; 

    double fps = m_project->media->fps();
    int64_t duration = m_project->media->duration();
    QString mediaPath = m_project->media->filePath();

    ProjectExportThread* exportThread = new ProjectExportThread(selectedFormat, p_timeline->getTimelineData(), fps, duration, mediaPath, selectedPath, this);

    QProgressDialog* progressDialog = new QProgressDialog(txtManager.get("export_running"), txtManager.get("generic_dialog_btn_cancel"), 0, 100, nullptr);
    progressDialog->show();


    connect(exportThread, &ProjectExportThread::progress, progressDialog, &QProgressDialog::setValue);
    
    connect(progressDialog, &QProgressDialog::canceled, this, [exportThread](){ 
        exportThread->requestInterruption();
    });

    connect(exportThread, &ProjectExportThread::exportFinished, this, [exportThread, progressDialog](bool success) {
        if (success) {
            qDebug() << "Export réussi";
        }else {
            qDebug() << "Export annulé ou erreur";
        }
        progressDialog->close(); 
        progressDialog->deleteLater(); 
        exportThread->deleteLater(); 
    });

    exportThread->start();

    qDebug() << "Export";
    // ProjectExportHandler::exportProject(m_project, p_timeline, ExportType::MP4, selectedPath);
}