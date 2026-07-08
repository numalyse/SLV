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

    const QVector<Annotation> &annotations() const;

    void setAnnotations(QVector<Annotation>* annotations);
    void clear();

public slots:
    void addAnnotation(Annotation& annotation);
    void updateAnnotation(const Annotation& annotation);
    void removeAnnotation(int annotationId);

signals:
    void annotationAdded(Annotation& annotation);
    void annotationUpdated(const Annotation& annotation);
    void annotationRemoved(int annotationId);
    void annotationsReset();

private:
    QVector<Annotation>* p_annotations = nullptr;
    int m_nextId = 0;
};


#endif //ANNOTATION_MANAGER_H