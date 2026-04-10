#ifndef SHORTCUTTAB_H
#define SHORTCUTTAB_H

#include "Preference/BasePreferenceTab.h"
#include "Preference/Frames/FormShortcutEditFrame.h"

#include <QScrollArea>
#include <QString>
#include <QMap>
#include <QJsonObject>

class ShortcutTab : public BasePreferenceTab
{
Q_OBJECT

public:
    explicit ShortcutTab(QWidget* parent = nullptr);

protected slots:
    void updateJsonObj(const QString& subCategory, const QString& key, const QString& newValue) override;

private slots:
    void emptyShortcutUI(const QString& stolenKey);

private:
    /// @brief Will check whether or not there is a conflict by traversing the updated jsonObject 
    /// @param newShortcut Value of the new shortcut
    /// @param currentKey Key of modified the shortcut to prevent conflict detection with itself
    /// @param outSubCategory Subcategory of the other shortcut that shares the same shortcut 
    /// @param outKey key of the other shortcut that shares the same shortcut 
    /// @return True if a conflict was found
    bool hasConflict(const QString& newShortcut, const QString& currentKey, QString& outSubCategory, QString& outKey);

    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames; // keeping this easily update on conflict

};


#endif