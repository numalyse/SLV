#ifndef PROJECT_H
#define PROJECT_H

#include "Shot.h"
#include "Media.h"

#include <QString>
#include <QImage>

#include <cstdint>

class Project : public QObject
{
Q_OBJECT

public:
    
    Project(Media* media, QObject *parent = nullptr);


private:
    QVector<Shot> m_shots;
    Media* m_media = nullptr;
    QString m_name;
    QString m_path;

private slots:
    void createProject(int64_t);

};


#endif