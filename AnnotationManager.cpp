#include "AnnotationManager.h"

#include "Project/ProjectManager.h"

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
        auto result = std::max_element(annotations->cbegin(), annotations->cend(), [](const Annotation& annotA, const Annotation& annotB){
            return annotA.id < annotB.id;
        });
        m_nextId = result->id;

        std::sort(p_annotations->begin(), p_annotations->end(), [](const Annotation& a, const Annotation& b){ 
            return a.start < b.start; 
        });
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

    validateAnnotation(annotation);

    // first element where start > annotation.end
    auto upperBound = std::upper_bound(p_annotations->cbegin(), p_annotations->cend(), annotation.end, [](int64_t annotEnd, const Annotation& b){
        return annotEnd < b.start;
    });

    // new annot before every other annot => add at head
    if(upperBound == p_annotations->cbegin()){
        annotation.id = ++m_nextId;
        p_annotations->insert(0, annotation);
        emit annotationAdded(annotation);
    } else if(upperBound == p_annotations->cend()){ // new annot after every other annot
        auto lowerBound = std::prev(upperBound);
        if(annotation.start > (*lowerBound).end){
            annotation.id = ++m_nextId;
            p_annotations->insert(upperBound, annotation); // insert at the end
            emit annotationAdded(annotation);
        }else {
            qDebug() << "[AnnotationManager] Failed to add the annotation";
            return;
        }
    }else { // new annot in the middle of the list
        auto lowerBound = std::prev(upperBound);
        if(annotation.start > (*lowerBound).end && annotation.end < (*upperBound).start){
            annotation.id = ++m_nextId;
            p_annotations->insert(upperBound, annotation); // insert before uperbound
            emit annotationAdded(annotation);
        }else {
            qDebug() << "[AnnotationManager] Failed to add the annotation";
            return;
        }
    }
}

void AnnotationManager::updateAnnotation(const Annotation &annotation)
{

    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to update on a nullptr "<< annotation.id;
        return;
    }

    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [&annotation](const Annotation& a){ return a.id == annotation.id; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to update, could not find annotation with id "<< annotation.id;
        return;
    }

    auto upperBound = std::next(it);

    bool leftMoved = (it->end == annotation.end);
    bool hasNext = (upperBound != p_annotations->end());
    bool hasPrev = (it != p_annotations->begin());

    // updates the annotation with the new data
    it->start = annotation.start;
    it->end = annotation.end;
    it->note = annotation.note;
    it->color = annotation.color;

    validateAnnotation(*it, leftMoved);

    // clamp start and end to neighbour
    if(hasNext && (*it).end > (*upperBound).start){
        it->end = (*upperBound).start - 1;
    }
    if(hasPrev){
        auto lowerBound = std::prev(it);
        if( (*it).start < (*lowerBound).end )
            it->start = (*lowerBound).end + 1;
    }

    // sends it back to timeline and panel to update ui
    emit annotationUpdated(*it);
}

void AnnotationManager::removeAnnotation(int annotationId)
{
    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to delete on a nullptr ";
        return;
    }

    auto it = std::find_if(p_annotations->cbegin(), p_annotations->cend(), [annotationId](const Annotation& a){ return a.id == annotationId; });
    if (it == p_annotations->cend()) {
        qDebug() << "[AnnotationManager] Failed to remove, could not find annotation with id "<< annotationId;
        return;
    }

    p_annotations->erase(it);

    emit annotationRemoved(annotationId);
}

void AnnotationManager::validateAnnotation(Annotation& annotation, bool leftMoved)
{
    Media* media = ProjectManager::instance().media();
    const int64_t duration = media ? media->duration() : 0;
    const int64_t minAnnotDur = (duration > 0) ? qMin(m_minAnnotDurationMs, duration) : m_minAnnotDurationMs;

    // check for value < 0
    annotation.start = qMax(annotation.start, int64_t{0});
    annotation.end = qMax(annotation.end, int64_t{0});

    // duration is set, clamp to duration
    if (duration > 0) {
        annotation.start = qMin(annotation.start, duration);
        annotation.end = qMin(annotation.end, duration);
    }

    // check if duration < min duration
    if (annotation.end - annotation.start < minAnnotDur ){
        // grows the annotation if needed
        if(leftMoved) {
            annotation.start = qMax(0, annotation.end - minAnnotDur);
            annotation.end = annotation.start + minAnnotDur;
        } else {
            annotation.end = annotation.start + minAnnotDur;
            if(duration > 0 && annotation.end > duration){
                annotation.end = duration;
                annotation.start = annotation.end - minAnnotDur;
            }
        }
    }
}

