#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QCheckBox>
#include <QGuiApplication>
#include <QStyleHints>
#include <QLayout>
#include <QToolBox>
#include <QTreeWidget>
#include <QStackedWidget>

class HelpDialog : public QDialog
{
Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

    void showContent(QWidget *widget);

protected:
    void initSideMenu();

private slots:

private:
    QWidget *m_helpDialogWidget = nullptr;
    QHBoxLayout *m_helpDialogLayout = nullptr;

    QWidget *m_sideMenuWidget = nullptr;
    QVBoxLayout *m_sideMenuLayout = nullptr;
    QTreeWidget *m_sideMenuTreeWidget = nullptr;

    QStackedWidget *m_contentWidget = nullptr;
};


#endif