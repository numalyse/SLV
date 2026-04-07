#ifndef FORMSHORTCUTEDITFRAME_H
#define FORMSHORTCUTEDITFRAME_H

#include <QFrame>
#include <QString>
#include <QWidget>

class FormShortcutEditFrame  : public QFrame
{
Q_OBJECT

public:
    FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent);

private:

};

#endif
