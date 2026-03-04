#ifndef SHOT_H
#define SHOT_H

#include <QString>
#include <QImage>

struct Shot
{
    QString title;
    int64_t start;
    int64_t end;
    QString note;
    QColor color;
    // image ?
};


#endif