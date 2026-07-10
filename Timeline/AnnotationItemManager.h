#ifndef ANNOTATIONITEMMANAGER_H
#define ANNOTATIONITEMMANAGER_H

#include "Timeline/TimelineView.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/Items/AnnotationItem.h"

#include <QObject>
#include <QGraphicsScene>
#include <QVector>

class AnnotationItemManager : public QObject
{
    Q_OBJECT

public:
    explicit AnnotationItemManager(QGraphicsScene *scene, TimelineView *view, TimelineMath *mathManager, QObject *parent);
    void updateAnnotItemsPosition();

public slots:
    void onAnnotationAdded(Annotation& annotation);
    void onAnnotationUpdated(const Annotation& annotation);
    void onAnnotationRemoved(int annotationId);

    /// @brief Remove all items from the scene, and recreate items based on project annotations
    void rebuild();

private:

    /// @brief Creates an annotation item then adds it to the scene and item list
    /// @param annotation 
    void createAnnotItem(const Annotation &annotation);

    QVector<AnnotationItem*> m_items;

    QGraphicsScene* p_scene = nullptr;
    TimelineView* p_view = nullptr;
    TimelineMath* p_mathManager = nullptr;

};


#endif