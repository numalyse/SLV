#include "Project/ProjectManager.h"

#include "Project/Project.h"
#include "Media.h"
#include "PrefManager.h"
#include "Project/ProjectFileHelper.h"
#include "Project/ProjectExportThread.h"
#include "FileCopyThread.h"
#include "MacSymLink.h"

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
#include <QDesktopServices>

#include <fstream>
#include <iostream>
#include "ProjectManager.h"


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
    PrefManager& prefManager = PrefManager::instance();
    switch (error) {
        case ProjectFileError::FolderNotFound:
            return prefManager.getText("project_manager_error_folder_not_found"); 
        case ProjectFileError::JsonFileNotFound:
            return prefManager.getText("project_manager_error_json_file_not_found");   
        case ProjectFileError::CannotOpenJsonFile:
            return prefManager.getText("project_manager_error_cannot_open_json_file");
        case ProjectFileError::JsonParsingError:
            return prefManager.getText("project_manager_error_json_parsing_error");
        case ProjectFileError::MediaFileNotFound:
            return prefManager.getText("project_manager_error_media_file_not_found"); 
        case ProjectFileError::MediaKeyMissing:
            return prefManager.getText("project_manager_error_media_key_missing"); 

        case ProjectFileError::UnexpectedError:
        default:
            return prefManager.getText("project_manager_error_unexpected_error");
    }
}

QString ProjectManager::getErrorMessage(ProjectManager::Error error) const {
    PrefManager& prefManager = PrefManager::instance();
    switch (error) {
        case ProjectManager::Error::MismatchFPS:
            return prefManager.getText("project_manager_error_media_mismatch_fps"); 
        case ProjectManager::Error::MismatchDuration:
            return prefManager.getText("project_manager_error_media_mismatch_duration"); 
        case ProjectManager::Error::UnexpectedError:
        default:
            return prefManager.getText("project_manager_error_unexpected_error");
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
    m_projectInitialized = false;

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


/// @brief Saves the current project, asks the user to input a foldername if project was never created, create and write to json, create a symbolic link to the media
/// @param ejectMediaAfterSave If set to true will eject the media and empty the current project 
/// @param isSaveAs Set to true if call to saveProject came from saveAs option, forces the creation of a new project folder and new media link
void ProjectManager::saveProject(bool ejectMediaAfterSave, bool isSaveAs){
    if(!m_project){
        qCritical() << "[ProjectManager] Trying to save on a null project"; 
        return;
    }

    if( ! m_needSave){ // If no changes made doesn't write to json
        if(ejectMediaAfterSave){
            discardAndEject();
        }
        return;
    }

    if( ! m_project->path.isEmpty() && ! isSaveAs ){ // If project path is not empty, the project folder as been created
        // we can directly write to json without creating a folder and the media link
        ProjectFileHelper::writeJson(m_project, p_timeline);
        setSaveNotNeeded();
        if(ejectMediaAfterSave){
            discardAndEject();
        }
        return;
    }

    if( mediaPath().isEmpty() ){
        qCritical() << "[ProjectManager] Media path empty when saving";
        return;
    }


    if ( ! createProjectFolder() ){ // clicked on "cancel" in filedialog returns
        return; 
    }else { 
/*         
        // copies the media file to the project folder

        QString destMedia = QDir(m_project->path).filePath(m_project->media->fileName() + "." + m_project->media->fileExtension());
        copyMedia(m_project->media->filePath(), destMedia, m_project->path, ejectMediaAfterSave); 
*/

        // on windows QFile::link creates a .lnk shortcut (tracks the target across moves),
        // on macOS we create a Finder alias which does the same; a plain POSIX symlink
        // (QFile::link on unix) breaks as soon as the target moves.
#ifdef Q_OS_WIN
        QString mediaLinkPath = QDir(m_project->path).filePath(m_project->media->fileName() + ".lnk" );
        bool linkCreated = QFile::link(m_project->media->filePath(), mediaLinkPath);
#elif defined(Q_OS_MACOS)
        QString mediaLinkPath = QDir(m_project->path).filePath(m_project->media->fileName() + "." + m_project->media->fileExtension());
        bool linkCreated = MacSymLink::create(m_project->media->filePath(), mediaLinkPath);
#else
        QString mediaLinkPath = QDir(m_project->path).filePath(m_project->media->fileName() + "." + m_project->media->fileExtension());
        bool linkCreated = QFile::link(m_project->media->filePath(), mediaLinkPath);
#endif

        if(!linkCreated){
            qCritical() << "[ProjectManager] Failed to create a link to the media";
            return;
        }
        m_project->mediaLinkPath = mediaLinkPath;

        if(!ProjectFileHelper::writeJson(m_project, p_timeline)){
            qCritical() << "[ProjectManager] Failed to write to json";
            return;
        }
        
        setSaveNotNeeded();

        if(ejectMediaAfterSave){
            discardAndEject();
        }

        return;
    }

}

Media* ProjectManager::media()
{
    if(m_project){
        return m_project->media;
    }
    return nullptr;
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

    if( ! m_isFpsParsed || ! m_isDurationParsed || m_projectInitialized ){
        return;
    }
    m_projectInitialized = true;

    Q_ASSERT(m_project);

    if(!m_project){
        qFatal() << "Création d'un plan sur un projet = nullptr ";
        return;
    }

    Shot shot{PrefManager::instance().getText("shot_detail_title_name"), 0, m_project->media->duration()};
    shot.tagImageTime = shot.middle();
    m_project->shots.append(shot);
    
    for (size_t i = 0; i < m_project->shots.size(); i++) {
        qDebug() << m_project->shots[i].title << ", " <<  m_project->shots[i].start << ", " << shot.end;
    }

    qDebug() << "project initialisé";
    emit projectInitialized();

}


bool ProjectManager::createProjectFolder(){
    PrefManager& prefManager = PrefManager::instance();
    QString fileType = prefManager.getText("project_manager_create_project_dialog_file_type") + "(*)";
    QString selectedPath = QFileDialog::getSaveFileName(
        nullptr, 
        tr(prefManager.getText("project_manager_create_project_dialog").toStdString().c_str()), 
        prefManager.getPref("Paths", "lp_project"),
        tr(fileType.toStdString().c_str() )
    );

    if(selectedPath.isEmpty()){
        qDebug() << "[ProjectManager] Project fodler creation aborted";
        return false; 
    }
    
    QFileInfo fileInfo(selectedPath);
    prefManager.setPref("Paths", "lp_project", fileInfo.absolutePath());

    QDir dir(fileInfo.absolutePath());
    
    if(!dir.exists(fileInfo.baseName())) {
        if(!dir.mkdir(fileInfo.baseName())) {
            qCritical() << "[ProjectManager] Failed to create project folder";
            return false;
        }
    }

    m_project->path = QDir(fileInfo.absolutePath()).filePath(fileInfo.baseName());
    m_project->name = QDir(m_project->path).dirName();
    qDebug() << "[ProjectManager] Project folder created";
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


/// @brief Ouvre un dialogue pour choisir l'emplacement du dossier. Parcours le JSON, affiche une erreur dans une message box s'il y en eu, créer projet sinon.
/// Puis parse le media
void ProjectManager::openProject()
{
    auto& prefManager = PrefManager::instance();

    QString selectedPath = QFileDialog::getExistingDirectory(
        nullptr, 
        tr(prefManager.getText("project_manager_open_project_dialog").toStdString().c_str()), 
        prefManager.getPref("Paths", "lp_project")
    );

    openProjectFromPath(selectedPath);

}

void ProjectManager::openProjectFromPath(const QString& path)
{
    auto& prefManager = PrefManager::instance();

    if(path.isEmpty()){
        return;
    }

    QFileInfo fileInfo(path);
    prefManager.setPref("Paths", "lp_project", fileInfo.absolutePath());

    auto loaded = ProjectFileHelper::loadProject(path);
    qDebug() << "Project loaded from path:" << path << ", success:" << loaded.has_value() << ", error:" << (loaded.has_value() ? "none" : getErrorMessage(loaded.error()));

    if (!loaded.has_value()) {
        QString errorMsg = getErrorMessage(loaded.error());
        QMessageBox::critical(nullptr, prefManager.getText("messagebox_error"), errorMsg);
        return;
    }

    ProjectSaveData projectData = loaded.value();

    QString mediaName;
    QString mediaAbsolutePath;

    Project* project = new Project{
        projectData.shots,
        new Media(projectData.mediaAbsolutePath, this),
        QFileInfo(path).baseName(),
        path,
        projectData.mediaLinkAbsolutePath
    };
    qDebug() << "Project created with name:" << project->name << ", path:" << project->path << ", media path:" << project->media->filePath();

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
            QMessageBox::critical(nullptr, PrefManager::instance().getText("messagebox_error"), errorMsg);
        }

    });

    connect(m_project->media, &Media::fpsParsed, this, [this, fpsJson = projectData.fps](double fpsFile) {

        if(fpsJson == fpsFile){
            m_isFpsParsed = true;
            checkMediaFullyLoaded();
        }else {
            QString errorMsg = getErrorMessage(ProjectManager::Error::MismatchFPS);
            QMessageBox::critical(nullptr, PrefManager::instance().getText("messagebox_error"), errorMsg);
        }

    });

    m_project->media->parse();
}

///@brief Quand les fps et la durée sont retrouvés, lance un signal pour créer un layout avec 1 player et lance un signal pour créer la timeline
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

    PrefManager& prefManager = PrefManager::instance();

    QString extension = '.' + mediaPathExtension();

    if ( extension.isEmpty() ) return;

    auto format = ProjectExportHelper::selectFormatWindow(m_project->media->type(), extension);

    if ( ! format.has_value() ) return;
    
    ExportType selectedFormat = format.value();

    // si on est dans un project existant (avec un dossier), on enregistre par défaut dans le dossier du projet
    // sinon on recupère le path dans les preferences
    QString dialogDir = (m_project->path.isEmpty()) ? prefManager.getPref("Paths", "lp_export"): m_project->path;

    QString selectedPath;
    if(selectedFormat == ExportType::TagImage){
        selectedPath = QFileDialog::getExistingDirectory(
            nullptr, 
            prefManager.getText("export_directory_path_title"),
            dialogDir,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    }else {
        QString dialogFilter = prefManager.getText(SLV::getExportTypeString(selectedFormat)) ;
        if(selectedFormat == ExportType::SRC)
            dialogFilter.replace(".src", '.'+mediaPathExtension());
        selectedPath = QFileDialog::getSaveFileName(
            nullptr, 
            prefManager.getText("export_file_path_title"), 
            dialogDir+'/'+m_project->media->fileName() + "_" + SLV::getExportExtensionString(selectedFormat) + "_export",
            dialogFilter
        );
    }

    if ( selectedPath.isEmpty() ) return;
    
    if ( m_project->path.isEmpty()) { // si on a pas de path on met a jour les prefs 
        QFileInfo fileInfo(selectedPath);
        prefManager.setPref("Paths", "lp_export", fileInfo.absolutePath());
    }


    double fps = m_project->media->fps();
    int64_t duration = m_project->media->duration();
    QString mediaPath = m_project->media->filePath();

    ProjectExportThread* exportThread = new ProjectExportThread(selectedFormat, p_timeline->getTimelineData(), fps, duration, mediaPath, m_project->media->sar(), selectedPath.split(".")[0], this);

    QProgressDialog* progressDialog = new QProgressDialog(prefManager.getText("export_running"), prefManager.getText("generic_dialog_btn_cancel"), 0, 100, nullptr);
    progressDialog->show();


    connect(exportThread, &ProjectExportThread::progress, progressDialog, &QProgressDialog::setValue);
    
    connect(progressDialog, &QProgressDialog::canceled, this, [exportThread](){ 
        exportThread->requestInterruption();
    });

    connect(exportThread, &ProjectExportThread::exportFinished, this, [exportThread, progressDialog, selectedPath, this](bool success) {
        disconnect(progressDialog, &QProgressDialog::canceled, nullptr, nullptr);
        if (success) {
            qDebug() << "Export réussi";
            QMessageBox msg;
            QPushButton *openDirBtn = msg.addButton(
                PrefManager::instance().getText("open_file_directory"),
                QMessageBox::AcceptRole);
            msg.setStandardButtons(QMessageBox::StandardButton::Ok);
            msg.setInformativeText(PrefManager::instance().getText("project_exportation_finished"));
            msg.setIcon(QMessageBox::Information);
            msg.exec();

            if (msg.clickedButton() == openDirBtn) {
                QFileInfo fi(selectedPath);
                QDesktopServices::openUrl(QUrl::fromLocalFile(fi.dir().path()));
            }
        }else {
            qDebug() << "Export annulé ou erreur";
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::StandardButton::Ok);
            msg.setInformativeText(PrefManager::instance().getText("project_exportation_error"));
            msg.setIcon(QMessageBox::Information);
            msg.adjustSize();
            msg.exec();
        }
        progressDialog->close(); 
        progressDialog->deleteLater(); 
    });

    connect(exportThread, &QThread::finished, exportThread, &QObject::deleteLater);

    exportThread->start();
}
