#ifndef FORMLINEEDIT_H
#define FORMLINEEDIT_H

#include <QLineEdit>


class FormLineEdit : public QLineEdit
{
Q_OBJECT

public:
    explicit FormLineEdit(const QString& text, bool editable, QWidget* parent = nullptr);

signals:
    void focusIn();
    void focusOut();

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
};


#endif