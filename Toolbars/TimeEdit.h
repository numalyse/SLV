#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QLineEdit>

#include <QAction>
class TimeEdit : public QLineEdit
{
Q_OBJECT

public:
    explicit TimeEdit( const QString& txt , QWidget* parent = nullptr);

private:
    QAction* m_action = nullptr;

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

signals:
    void focusIn();
    void focusOut();

};


#endif