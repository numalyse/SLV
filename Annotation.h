#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QString>
#include <QImage>

struct Annotation
{
    int64_t start;
    int64_t end;
    QString note;
    QColor color = QColor(79, 134, 198, 255);
    int id;
};


#endif //ANNOTATION_H
