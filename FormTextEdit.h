#ifndef FORMTEXTEDIT_H
#define FORMTEXTEDIT_H

#include <QTextEdit>


class FormTextEdit : public QTextEdit
{
Q_OBJECT

public:
    explicit FormTextEdit(const QString& text, bool editable, QWidget* parent = nullptr);

signals:
    void focusIn();
    void focusOut();

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
};


#endif