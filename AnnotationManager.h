#ifndef ANNOTATION_MANAGER_H
#define ANNOTATION_MANAGER_H

#include "Annotation.h"

#include <QObject>
#include <QVector>


class AnnotationManager : public QObject
{
Q_OBJECT

public:
    explicit AnnotationManager(QObject* parent);

    void setAnnotations(QVector<Annotation>* annotations) { p_annotations = annotations; };
    void clear() { p_annotations = nullptr; m_nextId = 0; }

public slots:
    void addAnnotation(Annotation& annotation);
    void updateAnnotation(int annotationId, const Annotation& annotation);
    void removeAnnotation(int annotationId);


signals:
    void annotationAdded(const Annotation& annotation);
    void annotationUpdated(const Annotation& annotation);
    void annotationRemoved(int annotationId);

private:
    QVector<Annotation>* p_annotations = nullptr;
    int m_nextId = 0;
};


#endif //ANNOTATION_MANAGER_H