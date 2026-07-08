#ifndef ANNOTATIONPANEL_H
#define ANNOTATIONPANEL_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>

#include "Annotation.h"
#include "ToolbarButtons/ToolbarButton.h"

class AnnotationPanel : public QWidget
{
Q_OBJECT

public:
    explicit AnnotationPanel(QWidget* parent);

signals:
    void addAnottationRequested(Annotation& annotation);
    void updateAnottationRequested(const Annotation& annotation);
    void removeAnnotationRequested(int annotationId);

private:
    QVector<QLabel*> m_labels;
    QVBoxLayout* m_layout = nullptr;
    ToolbarButton* m_addAnnotationBtn = nullptr;

private slots:
    void onAnnotationAdded(Annotation& annotation);
    void onAnnotationUpdated(const Annotation& annotation);
    void onAnnotationRemoved(int annotationId);
    void rebuild();

    void annotationCreationDialog();

};



#endif ANNOTATIONPANEL_H