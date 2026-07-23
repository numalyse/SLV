#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GlobalPlayerManager.h"
#include "Tutorial.h"

#include <vlc/vlc.h>

#ifdef Q_OS_MAC
#include "MacMouseTracker.h"
#endif

#include <QMainWindow>
#include <QSplitter>

class PlayerWidget;
class PlayerLayoutManager;
class QWidget;
class QPoint;
class QTimer;
class QMouseEvent;
class QEvent;

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
    void openMediaAction();
    void openProjectAction();
    void selectAndLoadMediaFiles();
    void enableFullscreenMain();
    void disableFullscreenMain();
    void disableNavPanel();
    void enableNavPanel();
    
signals:
    void windowMovedOrResizedRequested();
    void mouseMovedInMainWindow(const QPoint &pos);

protected:
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event);

private:
    void changeArrangementWithSaveCheck(PlayerLayoutArrangement arrangement);
    void openPrefWidget();
    void openHelperWidget();
    void openAboutWidget();

    void enableMouseTrackingRecursive(QWidget* widget);
    
    void restartFullscreenToolbarHideTimer();
    void stopFullscreenToolbarHideTimer();
    void hideFullscreenToolbars();

    Ui::MainWindow *ui;

    QSplitter* splitter = nullptr;
    QSplitter* splitter2 = nullptr;

    QToolBar* m_toolbarQt = nullptr;
    GlobalPlayerManager* m_globalPlayerManager = nullptr;
    QWidget* m_currentLayout = nullptr;
    ToolbarToggleHoverButton *m_navPanelBtn = nullptr; // Utilisation d'un toggle button pour le bouton de menu
    ToolbarButton *m_playlistBtn = nullptr;
    ToolbarButton *m_shotDetailBtn = nullptr;
    ToolbarButton *m_annotationPanelBtn = nullptr;
    ToolbarToggleHoverButton *m_viewGridBtn = nullptr;
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

    QTimer* m_fullscreenToolbarHideTimer = nullptr;
    int m_fullscreenToolbarHideDelayMs = 1000;
    // QVector<PlayerWidget*> m_players;

    Tutorial* m_tuto = nullptr;
};
#endif // MAINWINDOW_H
