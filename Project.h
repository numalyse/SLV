#ifndef PROJECT_H
#define PROJECT_H

#include "Shot.h"
#include "Media.h"

#include <QString>
#include <QImage>

struct Project
{
    QVector<Shot> shots;
    Media media;
    QString name;
    QString path;
};


#endif