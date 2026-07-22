#include "Project/ProjectManager.h"

#include "Project/Project.h"
#include "Media.h"
#include "PrefManager.h"
#include "Project/ProjectFileHelper.h"
#include "Project/ProjectExportThread.h"
#include "FileCopyThread.h"
#include "FileFormatManager.h"

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
    m_annotationManager = new AnnotationManager(this);

    connect(m_annotationManager, &AnnotationManager::annotationAdded, this, &ProjectManager::setSaveNeeded);
    connect(m_annotationManager, &AnnotationManager::annotationUpdated, this, &ProjectManager::setSaveNeeded);
    connect(m_annotationManager, &AnnotationManager::annotationRemoved, this, &ProjectManager::setSaveNeeded);
}

ProjectManager::~ProjectManager()
{
    // if threads are still running waits for them to end, 
    // otherwise would crash as deleting a running QThread is undefined behavior
    for (QThread* thread : {m_exportThread.data(), m_fileCpyThread.data()}) {
        if (thread && thread->isRunning()) {
            thread->requestInterruption();
            thread->wait();
        }
    }

    if(m_project){
        delete m_project;
        m_project = nullptr;
    }
}

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


void ProjectManager::deleteProject() {
    if (m_project) {
        if (m_project->media) {
            m_project->media->deleteLater(); 
        }
        delete m_project;
        m_project = nullptr;
        p_timeline = nullptr;
        setSaveNotNeeded();
        m_annotationManager->clear();
        emit projectDeleted();
    }
}


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

        // on windows QFile::link creates a .lnk shortcut, to track media move see WinSymLink.h
        // on macOS we create a Finder alias which does the same.
#ifdef Q_OS_WIN
        QString mediaLinkPath = QDir(m_project->path).filePath(m_project->media->fileName() + ".lnk" );
#else
        QString mediaLinkPath = QDir(m_project->path).filePath(m_project->media->fileName() + "_link." + m_project->media->fileExtension());
#endif

        if(!ProjectFileHelper::createMediaLink(m_project->media->filePath(), mediaLinkPath)){
            qCritical() << "[ProjectManager] Failed to create a link to the media";
            return;
        }
        m_project->mediaLinkPath = mediaLinkPath;

        if(!ProjectFileHelper::writeJson(m_project, p_timeline)){
            qCritical() << "[ProjectManager] Failed to write to json";
            return;
        }
        
        setSaveNotNeeded();

        // information message box when project saved for the first time 
        auto& prefManager = PrefManager::instance();
        QMessageBox::information(nullptr, prefManager.getText("project_saved_title"), prefManager.getText("project_saved_text"));

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

void ProjectManager::initProjectShot(){

    // if fps and duration are not parsed or already initialized do nothing
    if( ! m_isFpsParsed || ! m_isDurationParsed || m_projectInitialized ){
        return;
    }
    m_projectInitialized = true;

    Q_ASSERT(m_project);

    if(!m_project){
        qFatal() << "Création d'un plan sur un projet = nullptr ";
        return;
    }

    // shot spanning the whole media
    Shot shot{"", 0, m_project->media->duration()};
    shot.tagImageTime = shot.middle();
    m_project->shots.append(shot);

    qDebug() << "project initialisé";
    emit projectInitialized();

    m_annotationManager->setAnnotations(&m_project->annotations);
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
        qDebug() << "[ProjectManager] Project folder creation aborted";
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




void ProjectManager::openProject()
{
    auto& prefManager = PrefManager::instance();

    QString selectedPath = QFileDialog::getExistingDirectory(
        nullptr, 
        tr(prefManager.getText("project_manager_open_project_dialog").toStdString().c_str()), 
        prefManager.getPref("Paths", "lp_project")
    );

    if(selectedPath.isEmpty()){
        return;
    }

    openProjectFromPath(selectedPath);

}

void ProjectManager::openProjectFromPath(const QString& path)
{
    auto& prefManager = PrefManager::instance();

    QFileInfo fileInfo(path);
    prefManager.setPref("Paths", "lp_project", fileInfo.absolutePath());

    auto loaded = ProjectFileHelper::loadProject(path);
    qDebug() << "Project loaded from path:" << path << ", success:" << loaded.has_value() << ", error:" << (loaded.has_value() ? "none" : getErrorMessage(loaded.error()));

    if (!loaded.has_value()) {
        QString errorMsg = getErrorMessage(loaded.error());
        if(loaded.error() != ProjectFileError::JsonFileNotFound) {

            QMessageBox::critical(nullptr, prefManager.getText("messagebox_error"), errorMsg);
            return;
        }
        if (!relinkJson(errorMsg, path)) { // ask to relink if error is JsonFileNotFound
            return;
        }
        // reloads after relink json success
        loaded = ProjectFileHelper::loadProject(path);
        if (!loaded.has_value()) {
            QMessageBox::critical(nullptr, prefManager.getText("messagebox_error"),
                getErrorMessage(loaded.error()));
            return;
        }
    }
    ProjectSaveData projectData = loaded.value();

    // the media link is broken
    // ask the user to locate the media again and recreate the link
    if (projectData.mediaAbsolutePath.isEmpty() && !relinkMedia(path, projectData)) {
        return;
    }

    QString mediaName;
    QString mediaAbsolutePath;

    Project* project = new Project{
        projectData.shots,
        projectData.annots,
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
    m_durationError = false;
    m_isFpsParsed = false;
    m_fpsError = false;


    connect(m_project->media, &Media::durationParsed, this, [this, durationJson = projectData.duration ](int64_t durationFile) {
        m_isDurationParsed = true;
        if(durationJson != durationFile){
            m_durationError = true;
        }
        checkMediaFullyLoaded();

    });

    connect(m_project->media, &Media::fpsParsed, this, [this, fpsJson = projectData.fps](double fpsFile) {
        m_isFpsParsed = true;
        if(fpsJson != fpsFile){
            m_fpsError = true;
        }
        checkMediaFullyLoaded();

    });

    m_project->media->parse();
}

bool ProjectManager::relinkMedia(const QString& projectPath, ProjectSaveData& projectData)
{
    auto& prefManager = PrefManager::instance();

    QMessageBox warningBox(QMessageBox::Warning,
        prefManager.getText("messagebox_error"),
        prefManager.getText("project_manager_relink_media_message"));
    warningBox.addButton(prefManager.getText("project_manager_relink_media_button"), QMessageBox::AcceptRole);
    warningBox.exec();

    QString newMediaPath = QFileDialog::getOpenFileName(
        nullptr,
        prefManager.getText("project_manager_relink_media_dialog"),
        prefManager.getPref("Paths", "lp_open_media"),
        FileFormatManager::instance().getOpenFileDialogFilters()
    );

    if (newMediaPath.isEmpty()) { // clicked on "cancel", abort the project load
        return false;
    }

    // rebuild the link path inside the current project folder, in case the
    // project folder itself was moved since the json was written
    QString linkName = QFileInfo(projectData.mediaLinkAbsolutePath).fileName();
    if (linkName.isEmpty()) {
#ifdef Q_OS_WIN
        linkName = QFileInfo(newMediaPath).completeBaseName() + ".lnk";
#else
        linkName = QFileInfo(newMediaPath).fileName();
#endif
    }
    QString linkPath = QDir(projectPath).filePath(linkName);

    if (!ProjectFileHelper::createMediaLink(newMediaPath, linkPath)) {
        qCritical() << "[ProjectManager] relink media : failed to recreate the media link";
        QMessageBox::critical(nullptr,
            prefManager.getText("messagebox_error"),
            getErrorMessage(ProjectFileError::UnexpectedError));
        return false;
    }

    projectData.mediaAbsolutePath = newMediaPath;
    projectData.mediaLinkAbsolutePath = linkPath;
    projectData.mediaName = QFileInfo(newMediaPath).fileName();

    return true;
}

bool ProjectManager::relinkJson(const QString& errorJson, const QString& projectPath)
{
    auto& prefManager = PrefManager::instance();

    QMessageBox warningBox(QMessageBox::Warning,
        prefManager.getText("messagebox_error"),
        errorJson + '\n' + prefManager.getText("project_manager_relink_json_txt"));
    warningBox.addButton(prefManager.getText("project_manager_relink_media_button"), QMessageBox::AcceptRole);
    warningBox.exec();

    QString jsonPath = QFileDialog::getOpenFileName(
        nullptr,
        prefManager.getText("project_manager_relink_json_dialog"),
        projectPath,
        "JSON (*.json)"
    );

    if (jsonPath.isEmpty()) { // clicked on "cancel", abort the project load
        return false;
    }

    QString projectName = QFileInfo(projectPath).baseName();

    bool renamed = QFile::rename(jsonPath, projectPath + QDir::separator() + projectName + ".json" );

    if(!renamed){
        QMessageBox warningBox(QMessageBox::Warning,
            prefManager.getText("messagebox_error"),
            prefManager.getText("project_manager_relink_json_error_rename"));
        warningBox.addButton(prefManager.getText("project_manager_relink_media_button"), QMessageBox::AcceptRole);
        warningBox.exec();

        return false;
    }

    return true;
}

void ProjectManager::checkMediaFullyLoaded()
{
    if (!m_project || !m_project->media) return;
    
    if(m_isDurationParsed && m_isFpsParsed && !m_fpsError && !m_durationError){

        const QStringList paths {m_project->media->filePath()};

        emit loadMediaProjectRequested(paths);
        emit projectInitialized();
        
        m_annotationManager->setAnnotations(&m_project->annotations);

    } else if(m_isDurationParsed && m_isFpsParsed){
        // if fps or duration mismatched occured, prompt the user
        QStringList errors;
        if(m_durationError) errors << getErrorMessage(ProjectManager::Error::MismatchDuration);
        if(m_fpsError) errors << getErrorMessage(ProjectManager::Error::MismatchFPS);
        QMessageBox::critical(nullptr, PrefManager::instance().getText("messagebox_error"), errors.join('\n'));

        // delete the link file, on next open a dialog to create a new link
        QFile::remove(m_project->mediaLinkPath);

        deleteProject();
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

    bool hasAnnotations = !m_annotationManager->annotations().isEmpty();
    auto selection = ProjectExportHelper::selectFormatWindow(m_project->media->type(), extension, hasAnnotations);

    if ( ! selection.has_value() ) return;

    ExportType selectedFormat = selection->type;
    ExportSource source = selection->source;

    // if the project as been created (a folder exists on disk), open the dialogDir inside the project folder
    // else uses the lp_export from the preferences paths
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
        QString sourceSuffix = (source == ExportSource::Annotations) ? "_annotations" : "";
        selectedPath = QFileDialog::getSaveFileName(
            nullptr,
            prefManager.getText("export_file_path_title"),
            dialogDir+'/'+m_project->media->fileName() + sourceSuffix + "_" + SLV::getExportExtensionString(selectedFormat) + "_export",
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

    QVector<ExportItem> items = (source == ExportSource::Annotations)
        ? ProjectExportHelper::fromAnnotations(m_annotationManager->annotations())
        : ProjectExportHelper::fromShots(p_timeline->getTimelineData());
    ExportLabels labels = ProjectExportHelper::makeExportLabels(source);

    ProjectExportThread* exportThread = new ProjectExportThread(selectedFormat, items, labels, fps, duration, mediaPath, m_project->media->sar(), selectedPath.split(".")[0], this);
    m_exportThread = exportThread;

    QProgressDialog* progressDialog = new QProgressDialog(prefManager.getText("export_running"), prefManager.getText("generic_dialog_btn_cancel"), 0, 100, nullptr);
    progressDialog->show();


    connect(exportThread, &ProjectExportThread::progress, progressDialog, &QProgressDialog::setValue);
    
    connect(progressDialog, &QProgressDialog::canceled, exportThread, &QThread::requestInterruption);

    connect(exportThread, &ProjectExportThread::exportFinished, this, [progressDialog, selectedPath, this](bool success, bool canceled) {
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
        }else if (!canceled) { // if user canceled doens't show error dialog
            qDebug() << "Erreur lors de l'export";
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
