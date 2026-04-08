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

    /// @brief Empties the value of the key sequence edit and sets previous value to ""
    void clearShortcutUI();

    /// @brief Updates the value of the key sequence edit and set previous value to the shortcut given
    void setShortcutUI(const QString &shortcut);

signals :
    void updateJsonObjRequested(const QString& subCategory, const QString& key, const QString& newShortcutString);
    void emptyShortcutUIRequested(const QString& key);

private:
    QKeySequenceEdit* m_keySequenceEdit = nullptr;
    QString m_prevValue; // when assigning keys and conflict occurs, give to ability to set the key back to its state before the assignment 


};

#endif
