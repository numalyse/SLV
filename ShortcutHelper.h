#ifndef SHORTCUTHELPER_H
#define SHORTCUTHELPER_H

#include <QString>
#include <QPushButton>
#include <QWidget>
#include <QShortcut>
#include <QtAssert>
#include <QVector>
#include <QShortcut>

namespace SLV
{
    
    inline QShortcut* createGlobalButtonShortcut(QWidget* widget, const QString& keyString, QPushButton* button, bool autoRepeat = true) {
        Q_ASSERT( !keyString.isEmpty() );
        QWidget* mainWindow = widget->window(); // need to add this so shortcuts can be used event if this is hidden
        QShortcut* shortcut = new QShortcut(QKeySequence(keyString), mainWindow);
        shortcut->setContext(Qt::ApplicationShortcut); 
        shortcut->setAutoRepeat(autoRepeat);

        if (autoRepeat){
            QObject::connect(shortcut, &QShortcut::activated, button, &QPushButton::click);
        }else {
            QObject::connect(shortcut, &QShortcut::activated, button, &QPushButton::animateClick);
        }

        return shortcut;

    };

    inline void clearShortcuts(QVector<QShortcut*>& shortcuts){
        for (auto& shortcut : shortcuts) {
            if (shortcut) {
                delete shortcut; 
            }
        }
        shortcuts.clear();
    }

} // namespace SLV


#endif