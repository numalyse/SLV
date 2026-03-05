#include "Project.h"
#include "Media.h"

#include <QString>
#include <QDebug>


Project::Project(Media* media, QObject *parent) : QObject(parent)
{   
    m_media = new Media(media->filePath(), this);
    connect(m_media, &Media::durationParsed, this, &Project::createProject);
    m_media->parse();
}

/// @brief Créer un projet avec 1 plan de la durée de la vidéo
/// @param mediaDuration 
void Project::createProject(int64_t mediaDuration){
    Shot shot{"Plan 1", 0, mediaDuration};
    m_shots.append(shot);
    for (size_t i = 0; i < m_shots.size(); i++)
    {
        qDebug() << shot.title << ", " << shot.start << ", " << shot.end;
    }
    qDebug() << "project créé";
}