#include "AnnotationPanel.h"

#include "Project/ProjectManager.h"

AnnotationPanel::AnnotationPanel(QWidget *parent) : QWidget(parent)
{

    auto* annotations = ProjectManager::instance().annotationManager();

    connect(annotations, &AnnotationManager::annotationAdded,   this, &AnnotationPanel::onAnnotationAdded);
    connect(annotations, &AnnotationManager::annotationUpdated, this, &AnnotationPanel::onAnnotationUpdated);
    connect(annotations, &AnnotationManager::annotationRemoved, this, &AnnotationPanel::onAnnotationRemoved);
    connect(annotations, &AnnotationManager::annotationsReset, this, &AnnotationPanel::rebuild);

    connect(this, &AnnotationPanel::addAnottationRequested, annotations, &AnnotationManager::addAnnotation);

    m_layout = new QVBoxLayout(this);

    m_addAnnotationBtn = new ToolbarButton(this, "plus_white");
    connect(m_addAnnotationBtn, &QPushButton::clicked, this, &AnnotationPanel::annotationCreationDialog);

}


void AnnotationPanel::onAnnotationAdded(Annotation& annotation){
    QLabel* annotationId = new QLabel(this);
    annotationId->setText(QString::number(annotation.id));
    m_labels.append(annotationId);
    layout()->addWidget(annotationId);
}

void AnnotationPanel::onAnnotationUpdated(const Annotation& annotation){

}

void AnnotationPanel::onAnnotationRemoved(int annotationId){

}

void AnnotationPanel::rebuild(){
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    for(int IAnnotItem = 0; IAnnotItem < m_labels.size(); ++IAnnotItem){
        m_labels[IAnnotItem]->deleteLater();
    }
    
    m_labels.clear();

    for(auto&& annotation : annotations){
        QLabel* annotationId = new QLabel(this);
        annotationId->setText(QString::number(annotation.id));
        m_labels.append(annotationId);
        m_layout->addWidget(annotationId);
    }
}

void AnnotationPanel::annotationCreationDialog()
{
    qDebug() << "[AnnotationPanel] add annot clicked";
    Annotation annot{"titre", 0, 0, "notes"};
    emit addAnottationRequested(annot);
}
