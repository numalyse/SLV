#ifndef FORMSHORTCUTEDITFRAME_H
#define FORMSHORTCUTEDITFRAME_H

#include <QFrame>
#include <QString>
#include <QWidget>
#include <QKeySequenceEdit>

class FormShortcutEditFrame  : public QFrame
{
Q_OBJECT

public:
    FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent);

    void clearShortcutUI();

signals :
    void emptyShortcutOf(QString key);

private:
    QKeySequenceEdit* m_keySequenceEdit = nullptr;


};

#endif
