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
    void createViewGridBtn();

public slots:
    void openMediaFile();
    void enableFullscreenMain();
    void disableFullscreenMain();
    void disableNavPanel();
    void enableNavPanel();

private:
    Ui::MainWindow *ui;

    QSplitter* splitter = nullptr;
    QSplitter* splitter2 = nullptr;

    QToolBar* m_toolbarQt = nullptr;
    GlobalPlayerManager* m_globalPlayerManager = nullptr;
    QWidget* m_currentLayout = nullptr;
    ToolbarToggleButton *m_navPanelBtn = nullptr; // Utilisation d'un toggle button pour le bouton de menu
    ToolbarPopupButton *m_viewGridBtn = nullptr;
    ToolbarButton *m_view1 = nullptr;
    ToolbarToggleHoverButton *m_view2 = nullptr;
    ToolbarButton *m_view2H = nullptr;
    ToolbarButton *m_view2V = nullptr;
    ToolbarToggleHoverButton *m_view3 = nullptr;
    ToolbarButton *m_view3Left = nullptr;
    ToolbarButton *m_view3Top = nullptr;
    ToolbarButton *m_view3Right = nullptr;
    ToolbarButton *m_view3Bot = nullptr;
    ToolbarButton *m_view3HAlign = nullptr;
    ToolbarButton *m_view3VAlign = nullptr;
    ToolbarButton *m_view4 = nullptr;
    bool wasMaximized = false;
    // QVector<PlayerWidget*> m_players;

};
#endif // MAINWINDOW_H
