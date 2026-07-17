#ifndef ANNOTATIONDIALOG_H
#define ANNOTATIONDIALOG_H

#include "Annotation.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class AnnotationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnotationDialog( const int64_t startTime, QWidget* parent = nullptr);

    /// @brief Create an annotation base on the dialog inputs
    Annotation annotation() const;

    void accept() override;

private:
    void initUiLayout();
    void openColorPicker();
    void updateColorButton();

    Annotation m_annotation {};
    double m_fps = 1.0;
    int64_t m_maxDuration = 0;

    static constexpr int64_t s_baseDurationMs = 10000;

    QLineEdit* m_startEdit = nullptr;
    QLineEdit* m_endEdit = nullptr;
    QLineEdit* m_nameEdit = nullptr;
    QTextEdit* m_noteEdit = nullptr;
    int m_noteHeight = 80;
    QPushButton* m_colorBtn = nullptr;
    ToolbarButton* m_colorPickerBtn = nullptr;
    QPushButton* m_confirmBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;
};

#endif // ANNOTATIONDIALOG_H
