#ifndef ANNOTATIONWIDGET_H
#define ANNOTATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QFrame>
#include <QTextEdit>
#include <QLineEdit>

#include "Annotation.h"
#include "ToolbarButtons/ToolbarButton.h"

class AnnotationWidget : public QWidget
{
Q_OBJECT

public:
    explicit AnnotationWidget(QWidget *parent, const Annotation &annotation = {});
    void initStyle();
    int annotationId() const { return m_annotation.id; }
    const Annotation& annot() { return m_annotation; }
    
public slots:
    void updateAnnotation(const Annotation &annotation);

signals:
    void annotationClicked(int annotationId);
    void editAnnotationRequested(int annotationId);
    void removeAnnotationRequested(int annotationId);
    void updateAnnotationRequested(const Annotation &annotation);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    enum class Mode {
        Minimized,
        Extended,
        Edited
    };

    void refreshContent();
    void setMode(Mode mode);
    void applyEdits();
    void adjustNoteEditHeight();
    void openColorPicker();
    void updateColorButton();
    void openMenu();
    double currentFps() const;

    Annotation m_annotation;

    bool m_isDarkMode = true;
    QColor m_palbtnColor;
    QString m_palbtnColorStr;
 
    Mode m_mode = Mode::Minimized;
    QTimer* m_clickTimer = nullptr;
    bool m_ignoreNextRelease = false;

    QHBoxLayout* m_topRowLayout = nullptr;
    QVBoxLayout* m_timeLayout = nullptr;

    QFrame* m_colorBar = nullptr;
    QLabel* m_startLabel = nullptr;
    QLabel* m_endLabel = nullptr;
    QLabel* m_nameLabel = nullptr;
    QLabel* m_noteLabel = nullptr;
    ToolbarButton* m_updateBtn = nullptr;

    // edit mode
    QColor m_editColor;
    QLineEdit* m_startEdit = nullptr;
    QLineEdit* m_endEdit = nullptr;
    QLineEdit* m_nameEdit = nullptr;
    QTextEdit* m_noteEdit = nullptr;
    QPushButton* m_colorBtn = nullptr;
    ToolbarButton* m_confirmBtn = nullptr;
    ToolbarButton* m_cancelBtn = nullptr;
    ToolbarButton* m_deleteBtn = nullptr;
};

#endif // ANNOTATIONWIDGET_H
