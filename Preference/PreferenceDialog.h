#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "Preference/InterfaceTab.h"
#include "Preference/ShortcutTab.h"

#include <QDialog>
#include <QTabWidget>
#include <QCloseEvent> 
#include <QMessageBox> 

class PreferenceDialog : public QDialog
{
Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~PreferenceDialog() {};

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void save();
    void discard();

private:
    QTabWidget* m_tabWidget = nullptr;
    InterfaceTab* m_interfaceTab = nullptr;
    ShortcutTab* m_shortcutTab = nullptr;

};

#endif