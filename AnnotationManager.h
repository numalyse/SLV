#ifndef ANNOTATION_MANAGER_H
#define ANNOTATION_MANAGER_H

#include "Annotation.h"

#include <QObject>
#include <QVector>

#include <optional>


class AnnotationManager : public QObject
{
Q_OBJECT

public:
    explicit AnnotationManager(QObject* parent);

    const QVector<Annotation> &annotations() const;

    bool hasConflict(const Annotation& annotation) const;

    void setAnnotations(QVector<Annotation>* annotations);
    void clear();

public slots:
    void addAnnotation(Annotation& annotation);
    void updateAnnotation(const Annotation& annotation);
    void removeAnnotation(int annotationId);
    void resizeAnnotation(const Annotation& annotation);


signals:
    void annotationAdded(Annotation& annotation);
    void annotationUpdated(const Annotation& annotation);
    void annotationRemoved(int annotationId);
    void annotationsReset();

private:
    void validateAnnotation(Annotation& annotation, bool leftMoved = false);

    /// @brief Returns where the annotation would be inserted to keep the vector sorted,
    // or nullopt if it would overlap an existing annotation (itself excluded)
    /// @param annotation 
    /// @return 
    std::optional<QVector<Annotation>::iterator> findInsertPosition(const Annotation& annotation) const;

    const int64_t m_minAnnotDurationMs = 0;
    QVector<Annotation>* p_annotations = nullptr;
    int m_nextId = 0;
};


#endif //ANNOTATION_MANAGER_H