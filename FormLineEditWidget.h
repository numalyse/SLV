#ifndef FORMLINEEDITWIDGET_H
#define FORMLINEEDITWIDGET_H

#include "FormLineEdit.h"

#include <QFrame>
#include <QString>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>

class FormLineEditWidget : public QFrame
{
Q_OBJECT

public:
    explicit FormLineEditWidget(const QString& name, const QString& text, bool editable, QWidget* parent=nullptr);

    void setText(const QString& newText);

    FormLineEdit* lineEdit() { return m_lineEdit;}

private:
    QFormLayout* m_frameLayout = nullptr;
    QLabel* m_nameLabel = nullptr;
    FormLineEdit* m_lineEdit = nullptr;
};


#endif