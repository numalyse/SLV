#ifndef SHORTCUTTAB_H
#define SHORTCUTTAB_H

#include "Preference/FormShortcutEditFrame.h"

#include <QScrollArea>
#include <QString>
#include <QMap>
class ShortcutTab : public QScrollArea
{
Q_OBJECT

public:
    explicit ShortcutTab(QWidget* parent = nullptr);
    ~ShortcutTab() {}

private:
    QMap<QString, FormShortcutEditFrame*> m_shortcutFrames;
    
private slots:
    void onShortcutStolen(const QString& stolenKey);
};


#endif