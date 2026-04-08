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
    void setShortcutUI(const QString &shortcut);

signals :
    void updateJsonObjRequested(const QString& subCategory, const QString& key, const QString& newShortcutString);
    void emptyShortcutUIRequested(const QString& key);
private:
    QKeySequenceEdit* m_keySequenceEdit = nullptr;
    QString m_prevValue;


};

#endif
