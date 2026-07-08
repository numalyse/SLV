#include "AnnotationPanel.h"
#include "AnnotationDialog.h"

#include "Project/ProjectManager.h"

AnnotationPanel::AnnotationPanel(QWidget *parent) : QWidget(parent)
{

    auto* annotations = ProjectManager::instance().annotationManager();

    connect(annotations, &AnnotationManager::annotationAdded,   this, &AnnotationPanel::onAnnotationAdded);
    connect(annotations, &AnnotationManager::annotationUpdated, this, &AnnotationPanel::onAnnotationUpdated);
    connect(annotations, &AnnotationManager::annotationRemoved, this, &AnnotationPanel::onAnnotationRemoved);
    connect(annotations, &AnnotationManager::annotationsReset, this, &AnnotationPanel::rebuild);

    // once project fps are parsed, rebuild to update annotations fps
    connect(&ProjectManager::instance(), &ProjectManager::projectInitialized, this, &AnnotationPanel::rebuild);

    connect(this, &AnnotationPanel::addAnottationRequested, annotations, &AnnotationManager::addAnnotation);
    connect(this, &AnnotationPanel::updateAnottationRequested, annotations, &AnnotationManager::updateAnnotation);
    connect(this, &AnnotationPanel::removeAnnotationRequested, annotations, &AnnotationManager::removeAnnotation);

    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(4);
    m_layout->setAlignment(Qt::AlignTop);

    m_addAnnotationBtn = new ToolbarButton(this, "plus_white");
    connect(m_addAnnotationBtn, &QPushButton::clicked, this, &AnnotationPanel::annotationCreationDialog);

    m_layout->addWidget(m_addAnnotationBtn);

}

AnnotationWidget* AnnotationPanel::createItem(const Annotation& annotation)
{
    AnnotationWidget* item = new AnnotationWidget(this, annotation);

    connect(item, &AnnotationWidget::removeAnnotationRequested, this, &AnnotationPanel::removeAnnotationRequested);
    connect(item, &AnnotationWidget::editAnnotationRequested, this, &AnnotationPanel::annotationEditionDialog);
    connect(item, &AnnotationWidget::annotationClicked, this, &AnnotationPanel::annotationClicked);

    return item;
}

void AnnotationPanel::onAnnotationAdded(Annotation& annotation){
    AnnotationWidget* item = createItem(annotation);
    m_items.append(item);
    m_layout->addWidget(item);
}

void AnnotationPanel::onAnnotationUpdated(const Annotation& annotation){
    for(auto&& item : m_items){
        if(item->annotationId() == annotation.id){
            item->updateAnnotation(annotation);
            return;
        }
    }
}

void AnnotationPanel::onAnnotationRemoved(int annotationId){
    for(int IAnnotItem = 0; IAnnotItem < m_items.size(); ++IAnnotItem){
        if(m_items[IAnnotItem]->annotationId() == annotationId){
            m_items[IAnnotItem]->deleteLater();
            m_items.removeAt(IAnnotItem);
            return;
        }
    }
}

void AnnotationPanel::rebuild(){
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    for(int IAnnotItem = 0; IAnnotItem < m_items.size(); ++IAnnotItem){
        m_items[IAnnotItem]->deleteLater();
    }

    m_items.clear();

    for(auto&& annotation : annotations){
        AnnotationWidget* item = createItem(annotation);
        m_items.append(item);
        m_layout->addWidget(item);
    }
}

void AnnotationPanel::annotationCreationDialog()
{
    AnnotationDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted){
        Annotation annotation = dialog.annotation();
        emit addAnottationRequested(annotation);
    }
}

void AnnotationPanel::annotationEditionDialog(int annotationId)
{
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    for(auto&& annotation : annotations){
        if(annotation.id != annotationId)
            continue;

        AnnotationDialog dialog(this, annotation);
        if(dialog.exec() == QDialog::Accepted)
            emit updateAnottationRequested(dialog.annotation());

        return;
    }

    qDebug() << "[AnnotationPanel] edit requested, could not find annotation with id " << annotationId;
}
