#include "AnnotationItemManager.h"

#include "Project/ProjectManager.h"
#include "Timeline/Items/AnnotationHandleItem.h"
#include "Timeline/Items/TimelineLayerItem.h"

AnnotationItemManager::AnnotationItemManager(QGraphicsScene *scene, TimelineView *view, TimelineMath *mathManager, QObject *parent)
: QObject(parent), p_scene{scene}, p_view{view}, p_mathManager{mathManager}
{
    m_layer = new TimelineLayerItem();
    m_layer->setZValue(4);
    m_layer->setTransform(QTransform::fromScale(p_mathManager->pixelsPerMs(), 1.0));
    p_scene->addItem(m_layer);

    auto* annotations = ProjectManager::instance().annotationManager();

    connect(annotations, &AnnotationManager::annotationAdded,   this, &AnnotationItemManager::onAnnotationAdded);
    connect(annotations, &AnnotationManager::annotationUpdated, this, &AnnotationItemManager::onAnnotationUpdated);
    connect(annotations, &AnnotationManager::annotationRemoved, this, &AnnotationItemManager::onAnnotationRemoved);
    connect(annotations, &AnnotationManager::annotationsReset, this, &AnnotationItemManager::rebuild);

    // once project fps are parsed, rebuild to update annotations fps
    connect(&ProjectManager::instance(), &ProjectManager::projectInitialized, this, &AnnotationItemManager::rebuild);

    connect(this, &AnnotationItemManager::addAnnotationRequested, annotations, &AnnotationManager::addAnnotation);
    connect(this, &AnnotationItemManager::updateAnnotationRequested, annotations, &AnnotationManager::updateAnnotation);
    connect(this, &AnnotationItemManager::resizeAnnotationRequested, annotations, &AnnotationManager::resizeAnnotation);
    connect(this, &AnnotationItemManager::removeAnnotationRequested, annotations, &AnnotationManager::removeAnnotation);

}

void AnnotationItemManager::updateAnnotItemsPosition(){
    if(!m_layer) return;

    const double pixelsPerMs = p_mathManager->pixelsPerMs();

    m_layer->setTransform(QTransform::fromScale(pixelsPerMs, 1.0));
}

void AnnotationItemManager::onAnnotationAdded(Annotation& annotation){
    createAnnotItem(annotation);
}

void AnnotationItemManager::onAnnotationUpdated(const Annotation& annotation){
    auto it = std::find_if(m_items.cbegin(), m_items.cend(), [&annotation](const AnnotationItem* a){ return a->annotationId() == annotation.id; });
    if (it == m_items.cend()) {
        qDebug() << "[Timeline][AnnotationItemManager] onAnnotationUpdated : could not find annotation widget with id " << annotation.id;
        return;
    }

    (*it)->updateAnnotation(annotation);
    (*it)->setX(annotation.start);
    (*it)->setWidth(annotation.end - annotation.start);
}


void AnnotationItemManager::onAnnotationRemoved(int annotationId)
{
    if(!p_scene || !p_view) {
        qDebug() << "[Timeline][AnnotationItemManager] scene or view null, could not remove annotation item with id : " << annotationId;
        return;
    }

    auto it = std::find_if(m_items.cbegin(), m_items.cend(), [annotationId](const AnnotationItem* a){ return a->annotationId() == annotationId; });
    if (it == m_items.cend()) {
        qDebug() << "[Timeline][AnnotationItemManager] onAnnotationRemoved : could not find annotation widget with id " << annotationId;
        return;
    }

    delete (*it); 
    m_items.erase(it);
}



void AnnotationItemManager::rebuild(){
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    qDeleteAll(m_items);
    m_items.clear();

    for(auto&& annotation : annotations){
        createAnnotItem(annotation);
    } 
}

void AnnotationItemManager::onAnnotationHandleDragged(AnnotationHandleItem *handle, double posX)
{
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();
    const int id = handle->annotParent()->annotationId();

    auto it = std::find_if(annotations.cbegin(), annotations.cend(), [id](const Annotation& a){ return a.id == id; });
    if (it == annotations.cend()) {
        qDebug() << "[Timeline][AnnotationItemManager] handle dragged : could not find annotation with id " << id;
        return;
    }

    Annotation annot = *it;
    int64_t newTime = p_mathManager->posToTimeSnapped(posX);
    if (handle->isLeft())
        annot.start = newTime;
    else
        annot.end = newTime;

    emit resizeAnnotationRequested(annot);
}


void AnnotationItemManager::createAnnotItem(const Annotation& annotation)
{
    if(!p_scene || !p_view) {
        qDebug() << "[Timeline][AnnotationItemManager] scene or view null, failed to create annotation item with id : " << annotation.id;
        return;
    }

    AnnotationItem* item = new AnnotationItem(annotation, annotation.end - annotation.start, m_layer);

    m_items.append(item);
    item->setX(annotation.start);
}
