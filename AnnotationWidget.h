#ifndef ANNOTATIONWIDGET_H
#define ANNOTATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QFrame>

#include "Annotation.h"
#include "ToolbarButtons/ToolbarButton.h"

class AnnotationWidget : public QWidget
{
Q_OBJECT

public:
    explicit AnnotationWidget(QWidget *parent, const Annotation &annotation = {});
    void initStyle();
    int annotationId() const { return m_annotation.id; }

public slots:
    void updateAnnotation(const Annotation &annotation);

signals:
    void annotationClicked(int annotationId);
    void editAnnotationRequested(int annotationId);
    void removeAnnotationRequested(int annotationId);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void refreshContent();
    void openMenu();
    double currentFps() const;

    Annotation m_annotation;

    bool m_isDarkMode = true;
    QColor m_palbtnColor;
    QString m_palbtnColorStr;

    QFrame* m_colorBar = nullptr;
    QLabel* m_startLabel = nullptr;
    QLabel* m_endLabel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_noteLabel = nullptr;
    ToolbarButton* m_menuBtn = nullptr;
};

#endif // ANNOTATIONWIDGET_H
