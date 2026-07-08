#include "AnnotationManager.h"

#include <QDebug>

#include <algorithm>

AnnotationManager::AnnotationManager(QObject *parent)
: QObject(parent)
{
}

const QVector<Annotation>& AnnotationManager::annotations() const
{
    static const QVector<Annotation> empty;
    return p_annotations ? *p_annotations : empty;
}

void AnnotationManager::setAnnotations(QVector<Annotation> *annotations)
{
    p_annotations = annotations; 
    emit annotationsReset();
}

void AnnotationManager::clear()
{
    p_annotations = nullptr; 
    m_nextId = 0; 
    emit annotationsReset();
}

void AnnotationManager::addAnnotation(Annotation& annotation){
    annotation.id = ++m_nextId;
    p_annotations->append(annotation);

    emit annotationAdded(annotation);
}

void AnnotationManager::updateAnnotation(int annotationId, const Annotation &annotation)
{
    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [annotationId](const Annotation& a){ return a.id == annotationId; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to update, could not find annotation with id "<< annotationId;
        return;
    }

    // updates the annotation with the new data
    it->title = annotation.title;
    it->start = annotation.start;
    it->end = annotation.end;
    it->note = annotation.note;
    it->color = annotation.color;

    // sends it back to timeline and panel to update ui
    emit annotationUpdated(*it);
}

void AnnotationManager::removeAnnotation(int annotationId)
{
    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [annotationId](const Annotation& a){ return a.id == annotationId; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to remove, could not find annotation with id "<< annotationId;
        return;
    }

    p_annotations->erase(it);

    emit annotationRemoved(annotationId);
}

