#ifndef ANNOTATIONDIALOG_H
#define ANNOTATIONDIALOG_H

#include "Annotation.h"

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class AnnotationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnotationDialog(QWidget* parent = nullptr, const Annotation& annotation = {});

    /// @brief Create an annotation base on the dialog inputs
    Annotation annotation() const;

private:
    void initUiLayout();
    void openColorPicker();
    void updateColorButton();

    Annotation m_annotation;
    double m_fps = 25.0;
    int64_t m_maxDuration = 0;
    QColor m_color;

    QLineEdit* m_startEdit = nullptr;
    QLineEdit* m_endEdit = nullptr;
    QLineEdit* m_titleEdit = nullptr;
    QTextEdit* m_noteEdit = nullptr;
    QPushButton* m_colorBtn = nullptr;
    QPushButton* m_confirmBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;
};

#endif // ANNOTATIONDIALOG_H
