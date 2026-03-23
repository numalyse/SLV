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
    // if project 
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

    QString destMedia = m_project->path + "/" + m_project->name + "/" + m_project->media->fileName() + "."+ m_project->media->fileExtension();
    copyMedia(m_project->media->filePath(), destMedia, ejectMediaAfterSave);

    // créer json avec les infos du project : shots, media ect.*


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
        tr("Choisir l'emplacement et le nom du projet"), // TODO text Manager
        QDir::homePath(),
        tr("Projet (*)") 
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



bool ProjectManager::copyMedia(const QString& sourcePath, const QString& destPath, bool ejectMediaAfterSave) 
{ // QThread sinon ca va bloquer l'ui
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

    m_fileCpyThread = new FileCopyThread(sourcePath, destPath, this);

    // créer la fenetre pour le loading

    TextManager& txtManger = TextManager::instance();
    
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

    QString jsonPath = m_project->path +  "/" + m_project->name + "/" + m_project->name + ".json";

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
