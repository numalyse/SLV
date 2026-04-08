#ifndef SHORTCUTTAB_H
#define SHORTCUTTAB_H

#include "Preference/FormShortcutEditFrame.h"
#include "Preference/BasePreferenceTab.h"

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
    bool hasConflict(const QString& newShortcut, const QString& currentKey, QString& outSubCategory, QString& outKey);

    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames; // keeping this easily update on conflict

};


#endif