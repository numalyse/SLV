#include "AnnotationItemManager.h"
#include "Project/ProjectManager.h"


AnnotationItemManager::AnnotationItemManager(QGraphicsScene *scene, TimelineView *view, TimelineMath *mathManager, QObject *parent) 
: QObject(parent), p_scene{scene}, p_view{view}, p_mathManager{mathManager}
{
    auto* annotations = ProjectManager::instance().annotationManager();

    connect(annotations, &AnnotationManager::annotationAdded,   this, &AnnotationItemManager::onAnnotationAdded);
    connect(annotations, &AnnotationManager::annotationUpdated, this, &AnnotationItemManager::onAnnotationUpdated);
    connect(annotations, &AnnotationManager::annotationRemoved, this, &AnnotationItemManager::onAnnotationRemoved);
    connect(annotations, &AnnotationManager::annotationsReset, this, &AnnotationItemManager::rebuild);

}

void AnnotationItemManager::updateAnnotItemsPosition(){
    if(!p_scene || !p_view) {
        qDebug() << "[Timeline][AnnotationItemManager] scene or view null, could not update items position";
        return;
    }

    p_view->setUpdatesEnabled(false);

    double newXPos{};
    double newWidth{};

    for(auto* annotItem : m_items){

        newXPos = annotItem->annotation().start * p_mathManager->pixelsPerMs();
        newWidth = (annotItem->annotation().end - annotItem->annotation().start) * p_mathManager->pixelsPerMs();

        annotItem->setX(newXPos);
        annotItem->setWidth(newWidth);
    }

    p_view->setUpdatesEnabled(true);
    p_scene->update();
} 


void AnnotationItemManager::onAnnotationAdded(Annotation& annotation){
    createAnnotItem(annotation);
}

void AnnotationItemManager::onAnnotationUpdated(const Annotation& annotation){
    auto it = std::find_if(m_items.begin(), m_items.end(), [&annotation](const AnnotationItem* a){ return a->annotationId() == annotation.id; });
    if (it == m_items.end()) {
        qDebug() << "[Timeline][AnnotationItemManager] onAnnotationUpdated : could not find annotation widget with id " << annotation.id;
        return;
    }

    double startPos = p_mathManager->pixelsPerMs() * annotation.start;
    double endPos = p_mathManager->pixelsPerMs() * annotation.end;
    double newWidth = endPos - startPos;

    (*it)->updateAnnotation(annotation);
    (*it)->setX(startPos);
    (*it)->setWidth(newWidth);
}


void AnnotationItemManager::onAnnotationRemoved(int annotationId)
{
    if(!p_scene || !p_view) {
        qDebug() << "[Timeline][AnnotationItemManager] scene or view null, could not remove annotation item with id : " << annotationId;
        return;
    }

    auto it = std::find_if(m_items.begin(), m_items.end(), [annotationId](const AnnotationItem* a){ return a->annotationId() == annotationId; });
    if (it == m_items.end()) {
        qDebug() << "[Timeline][AnnotationItemManager] onAnnotationRemoved : could not find annotation widget with id " << annotationId;
        return;
    }

    p_scene->removeItem((*it));
    m_items.erase(it);
}



void AnnotationItemManager::rebuild(){
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    for(int IAnnotItem = 0; IAnnotItem < m_items.size(); ++IAnnotItem){
        auto* annotItem = m_items[IAnnotItem];
        p_scene->removeItem(annotItem);
    }

    m_items.clear();

    for(auto&& annotation : annotations){
        createAnnotItem(annotation);
    } 
}

void AnnotationItemManager::createAnnotItem(const Annotation& annotation)
{
    if(!p_scene || !p_view) {
        qDebug() << "[Timeline][AnnotationItemManager] scene or view null, failed to create annotation item with id : " << annotation.id;
        return;
    }

    double startPos = p_mathManager->pixelsPerMs() * annotation.start;
    double endPos = p_mathManager->pixelsPerMs() * annotation.end;
    double width = endPos - startPos;

    AnnotationItem* item = new AnnotationItem(annotation, width);

    m_items.append(item);
    p_scene->addItem(item);

    item->setX(startPos);
}
