#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QString>
#include <QImage>

struct Annotation
{
    int64_t start;
    int64_t end;
    QString name;
    QString note;
    QColor color = QColor{118, 1, 136, 255};
    int id;
};


#endif //ANNOTATION_H
