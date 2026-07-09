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
    if (!annotations || annotations->isEmpty()) {
        m_nextId = 0;
    } else {
        auto result = std::max_element(annotations->begin(), annotations->end(), [](const Annotation& annotA, const Annotation& annotB)
        {
            return annotA.id < annotB.id;
        });
        m_nextId = result->id;
    }
    emit annotationsReset();
}

void AnnotationManager::clear()
{
    p_annotations = nullptr; 
    m_nextId = 0; 
    emit annotationsReset();
}

void AnnotationManager::addAnnotation(Annotation& annotation){
    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to add on a nullptr";
        return;
    }
    annotation.id = ++m_nextId;
    p_annotations->append(annotation);

    emit annotationAdded(annotation);
}

void AnnotationManager::updateAnnotation(const Annotation &annotation)
{

    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to update on a nullptr "<< annotation.id;
        return;
    }

    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [annotation](const Annotation& a){ return a.id == annotation.id; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to update, could not find annotation with id "<< annotation.id;
        return;
    }

    // updates the annotation with the new data
    it->start = annotation.start;
    it->end = annotation.end;
    it->note = annotation.note;
    it->color = annotation.color;

    // sends it back to timeline and panel to update ui
    emit annotationUpdated(*it);
}

void AnnotationManager::removeAnnotation(int annotationId)
{
    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to delete on a nullptr ";
        return;
    }

    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [annotationId](const Annotation& a){ return a.id == annotationId; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to remove, could not find annotation with id "<< annotationId;
        return;
    }

    p_annotations->erase(it);

    emit annotationRemoved(annotationId);
}

