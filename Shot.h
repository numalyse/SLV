#ifndef SHOT_H
#define SHOT_H

#include <QString>
#include <QImage>
#include <QColor>

struct Shot
{
    QString title;
    int64_t start;
    int64_t end;
    QString note = "";
    QColor color = QColor(0, 0, 255, 127);
    // image ?
};


#endif