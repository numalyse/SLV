#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

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

private:
    QTabWidget* m_tabWidget = nullptr;
    bool m_needSave = false;
};

#endif