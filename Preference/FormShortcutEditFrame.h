#ifndef FORMSHORTCUTEDITFRAME_H
#define FORMSHORTCUTEDITFRAME_H

#include "Preference/BasePreferenceFrame.h"

#include <QFrame>
#include <QString>
#include <QWidget>
#include <QKeySequenceEdit>

class FormShortcutEditFrame  : public BasePreferenceFrame
{
Q_OBJECT

public:

    explicit FormShortcutEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent = nullptr);

    /// @brief Empties the value of the key sequence edit and sets previous value to ""
    void clearShortcutUI();

    /// @brief Updates the value of the key sequence edit and set previous value to the shortcut given
    void setUIValue(const QString &shortcut);

signals :
    void emptyShortcutUIRequested(const QString& key);

private:
    QKeySequenceEdit* m_keySequenceEdit = nullptr;
};

#endif
