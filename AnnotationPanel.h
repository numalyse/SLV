#ifndef ANNOTATIONPANEL_H
#define ANNOTATIONPANEL_H

#include <QWidget>
#include <QVector>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationWidget.h"
#include "ToolbarButtons/ToolbarButton.h"

class AnnotationPanel : public QWidget
{
Q_OBJECT

public:
    explicit AnnotationPanel(QWidget* parent);

signals:
    void addAnnotationRequested(Annotation& annotation);
    void updateAnnotationRequested(const Annotation& annotation);
    void removeAnnotationRequested(int annotationId);
    void annotationClicked(int annotationId);

private:
    void createItem(const Annotation& annotation);

    QVector<AnnotationWidget*> m_items;
    QVBoxLayout* m_layout = nullptr;
    ToolbarButton* m_addAnnotationBtn = nullptr;

private slots:
    void onAnnotationAdded(Annotation& annotation);
    void onAnnotationUpdated(const Annotation& annotation);
    void onAnnotationRemoved(int annotationId);
    void rebuild();

    void annotationCreationDialog();
    void annotationEditionDialog(int annotationId);

};



#endif // ANNOTATIONPANEL_H
