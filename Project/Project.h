#ifndef PROJECT_H
#define PROJECT_H

#include "Shot.h"
#include "Media.h"
#include "Annotation.h"

#include <QString>
#include <QImage>

struct Project
{
    QVector<Shot> shots;
    QVector<Annotation> annotations;
    Media* media = nullptr;
    QString name = "";
    QString path = "";
    QString mediaLinkPath = "";
};


#endif