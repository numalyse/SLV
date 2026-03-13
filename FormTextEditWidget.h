#ifndef FORMTEXTEDITWIDGET_H
#define FORMTEXTEDITWIDGET_H

#include "FormTextEdit.h"

#include <QFrame>
#include <QString>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

class FormTextEditWidget : public QFrame
{
Q_OBJECT

public:
    explicit FormTextEditWidget(const QString& name, const QString& text, bool editable, QWidget* parent=nullptr);

    void setText(const QString& newText);

    FormTextEdit* textEdit() { return m_textEdit;}

private:
    QVBoxLayout* m_frameLayout = nullptr;
    QLabel* m_nameLabel = nullptr;
    FormTextEdit* m_textEdit = nullptr;
};


#endif