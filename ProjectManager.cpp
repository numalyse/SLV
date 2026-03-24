#include "ProjectManager.h"

#include "Project.h"
#include "Media.h"

#include <QString>
#include <QDebug>
#include <QObject>

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
    m_project->media = new Media(media->filePath(), this, media->vlcInstance());
    connect(m_project->media, &Media::durationParsed, this, &ProjectManager::initProjectShot);
    m_project->media->parse();
}



void ProjectManager::deleteProject() { 
    delete m_project;
    m_project = nullptr;
    emit projectDeleted();
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

