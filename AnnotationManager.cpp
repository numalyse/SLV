#include "AnnotationManager.h"

#include "Project/ProjectManager.h"

#include <QDebug>

#include <algorithm>
#include <cmath>

AnnotationManager::AnnotationManager(QObject *parent)
: QObject(parent)
{
}

const QVector<Annotation>& AnnotationManager::annotations() const
{
    static const QVector<Annotation> empty;
    return p_annotations ? *p_annotations : empty;
}

std::optional<Annotation> AnnotationManager::findConflict(Annotation &annotation)
{
    if (!p_annotations)
        return std::nullopt;

    validateAnnotation(annotation);

    for (const Annotation& existing : *p_annotations) {
        if (existing.id == annotation.id)
            continue;
        if (existing.start < annotation.end && annotation.start < existing.end)
            return existing;
    }

    return std::nullopt;
}

std::optional<QVector<Annotation>::iterator> AnnotationManager::findInsertPosition(const Annotation &annotation) const
{
    if (!p_annotations)
        return std::nullopt;

    for (const Annotation& existing : *p_annotations) {
        if (existing.id == annotation.id)
            continue;
        if (existing.start < annotation.end && annotation.start < existing.end)
            return std::nullopt; // overlap
    }

    return std::lower_bound(p_annotations->begin(), p_annotations->end(), annotation.start,
                            [](const Annotation& existing, int64_t annotStart) {
                                return existing.start < annotStart;
                            });
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

    // where the annotation would end up, nullopt if it overlaps another annotation
    auto insertPos = findInsertPosition(annotation);
    if (!insertPos) {
        qDebug() << "[AnnotationManager] Failed to add the annotation, overlap";
        return;
    }

    annotation.id = ++m_nextId;
    p_annotations->insert(*insertPos, annotation);
    emit annotationAdded(annotation);
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

    Annotation updated = annotation;
    validateAnnotation(updated);

    // where the annotation would end up, nullopt if it overlaps another annotation
    auto insertPos = findInsertPosition(updated);
    if (!insertPos) {
        qDebug() << "[AnnotationManager] Failed to update, overlap";
        return;
    }

    auto updatedUpperBound = *insertPos;

    *it = updated;

    // move the updated annot before updatedUpperBound, keeping the vector sorted
    if (it < updatedUpperBound) { // if moved to right 
        std::rotate(it, std::next(it), updatedUpperBound);
        it = std::prev(updatedUpperBound);
    } else if (updatedUpperBound < it) { // moved to left
        std::rotate(updatedUpperBound, it, std::next(it)); 
        it = updatedUpperBound;
    }

    emit annotationUpdated(*it);

}

void AnnotationManager::resizeAnnotation(const Annotation &annotation)
{
    if(!p_annotations){
        qDebug() << "[AnnotationManager] Failed to resize on a nullptr "<< annotation.id;
        return;
    }

    auto it = std::find_if(p_annotations->begin(), p_annotations->end(), [&annotation](const Annotation& a){ return a.id == annotation.id; });
    if (it == p_annotations->end()) {
        qDebug() << "[AnnotationManager] Failed to resize, could not find annotation with id "<< annotation.id;
        return;
    }

    auto upperBound = std::next(it);
    const int64_t originalStart = it->start;
    const int64_t originalEnd = it->end;

    bool leftMoved = (it->end == annotation.end);
    bool hasNext = (upperBound != p_annotations->end());
    bool hasPrev = (it != p_annotations->begin());

    // updates the annotation with the new data
    it->start = annotation.start;
    it->end = annotation.end;
    it->name = annotation.name;
    it->note = annotation.note;
    it->color = annotation.color;

    validateAnnotation(*it, leftMoved);

    // clamp start and end to neighbour
    if(hasNext && (*it).end > (*upperBound).start){
        it->end = (*upperBound).start;
    }
    if(hasPrev){
        auto lowerBound = std::prev(it);
        if( (*it).start < (*lowerBound).end )
            it->start = (*lowerBound).end;
    }

    Media* media = ProjectManager::instance().media();
    const int64_t duration = media ? media->duration() : 0;
    if (it->end - it->start < minimumAnnotationDurationMs(duration)) {
        it->start = originalStart;
        it->end = originalEnd;
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
    const int64_t minAnnotDur = minimumAnnotationDurationMs(duration);

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

int64_t AnnotationManager::minimumAnnotationDurationMs(int64_t mediaDuration) const
{
    Media* media = ProjectManager::instance().media();
    const double fps = media ? media->fps() : 0.0;
    const int64_t frameDurationMs = fps > 0.0
        ? std::max<int64_t>(1, static_cast<int64_t>(std::ceil(1000.0 / fps)))
        : 1;
    return mediaDuration > 0 ? qMin(frameDurationMs, mediaDuration) : frameDurationMs;
}
