#ifndef SHOT_H
#define SHOT_H

#include <QString>
#include <QImage>

struct Shot
{
    QString title;
    int64_t start;
    int64_t end;
    int64_t tagImageTime;
    QString note;
    QColor color = QColor(79, 134, 198, 255);
    QColor borderColor = QColor(33, 71, 115, 255);

    int64_t middle() {
        return start + (end - start) / 2;
    }
    
};

struct AudioShot : Shot
{
    AudioShot() {
        color = QColor(108, 73, 184, 255);
        borderColor = QColor(64, 39, 117, 255);
    }
};


#endif
