#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GlobalPlayerManager.h"
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
    void createMenuBar();
    void createToolBar();

public slots:
    void openMediaFile();
    void enableFullscreenMain();
    void disableFullscreenMain();

private:
    Ui::MainWindow *ui;

    QSplitter* splitter = nullptr;
    QSplitter* splitter2 = nullptr;

    QToolBar* m_toolbarQt = nullptr;
    GlobalPlayerManager* m_globalPlayerManager = nullptr;
    QWidget* m_currentLayout = nullptr;
    ToolbarToggleButton *m_navPanelBtn = nullptr; // Utilisation d'un toggle button pour le bouton de menu
    // QVector<PlayerWidget*> m_players;

};
#endif // MAINWINDOW_H
