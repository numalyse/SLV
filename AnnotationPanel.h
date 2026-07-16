#ifndef ANNOTATIONPANEL_H
#define ANNOTATIONPANEL_H

#include <QWidget>
#include <QVector>
#include <QVBoxLayout>
#include <QColor>

#include "Annotation.h"
#include "AnnotationWidget.h"
#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"

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
    void createItem(const Annotation& annotation, bool checkOrder);
    void filterBy(const QVector<QColor>& color);

    QVector<AnnotationWidget*> m_items;
    QVBoxLayout* m_layout = nullptr;
    QVBoxLayout* m_itemsLayout = nullptr;
    ToolbarButton* m_addAnnotationBtn = nullptr;
    ToolbarToggleHoverButton* m_filterByColorBtn = nullptr;
    QVector<QColor> m_filterColors;

private slots:
    void onAnnotationAdded(Annotation& annotation);
    void onAnnotationUpdated(const Annotation& annotation);
    void onAnnotationRemoved(int annotationId);
    void rebuild();

    void annotationCreationDialog();
};



#endif // ANNOTATIONPANEL_H
