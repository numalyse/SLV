#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "Preference/BasePreferenceTab.h"

#include <QDialog>
#include <QTabWidget>
#include <QCloseEvent> 
#include <QMessageBox> 
#include <QDialogButtonBox>

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
    void toggleButtons();

private:
    bool needSave();
    void showWarning();

    QTabWidget* m_tabWidget = nullptr;
    QVector<BasePreferenceTab*> m_tabs;
    QDialogButtonBox* m_buttonBox = nullptr;
};

#endif