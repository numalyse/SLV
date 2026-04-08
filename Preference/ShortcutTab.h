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
    bool needSave();
    void save();
    void discard();

    ~ShortcutTab() {}

private:
    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames;
    QJsonObject m_baseShortcut;
    QJsonObject m_updatedShortcut;

private slots:
    void emptyShortcutUI(const QString& stolenKey);
    void updateJsonObj(const QString& subCategory, const QString& key, const QString& newShortcutString);
};


#endif