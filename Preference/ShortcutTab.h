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

private:
    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames; // keeping this easily update on conflict

private slots:
    void emptyShortcutUI(const QString& stolenKey);
};


#endif