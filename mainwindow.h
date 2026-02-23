#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PlayerWidget.h"
#include <vlc/vlc.h>

#include <QMainWindow>
#include <QSplitter>

class PlayerWidget;
class PlayerLayoutManager;
class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setPlayers(int count);

private:
    Ui::MainWindow *ui;

    QSplitter* splitter = nullptr;
    QSplitter* splitter2 = nullptr;

    PlayerLayoutManager* m_layoutManager = nullptr;
    QWidget* m_currentLayout = nullptr;
    QVector<PlayerWidget*> m_players;

};
#endif // MAINWINDOW_H
