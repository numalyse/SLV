#ifndef SHORTCUTTAB_H
#define SHORTCUTTAB_H

#include "Preference/FormShortcutEditFrame.h"

#include <QScrollArea>
#include <QString>
#include <QMap>
#include <QJsonObject>
class ShortcutTab : public QScrollArea
{
Q_OBJECT

public:
    explicit ShortcutTab(QWidget* parent = nullptr);

    /// @brief Checks whether one of the shortcut has been modified and needs to ask user to save
    /// @return True if needs save
    bool needSave();

    /// @brief Updates the json with all of the changes from this tab
    void save();

    /// @brief Reverts all changes of all tabs
    void discard();

    ~ShortcutTab() {}

private:
    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames; // map to easily access and update the value of each shortcuts (based on the key in the json ex : "enter_fullscreen") 

    // use of 2 json object to check if needing to save and be able to discard changes
    QJsonObject m_baseShortcut;
    QJsonObject m_updatedShortcut;

private slots:
    void emptyShortcutUI(const QString& stolenKey);
    void updateJsonObj(const QString& subCategory, const QString& key, const QString& newShortcutString);
};


#endif