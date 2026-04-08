#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "GlobalPlayerManager.h"
#include "Project/ProjectManager.h"
#include "PlayerWidget.h"
#include "PrefManager.h"
#include "GenericDialog.h"
#include "Preference/PreferenceDialog.h"

#include <qtoolbar.h>
#include <vlc/vlc.h>

#include <QTimer>
#include <QFile>
#include <QUrl>
#include <QByteArray>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QIcon>
#include <QDir>
#include <QAction>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setCentralWidget(ui->centralwidget);

    // ===== Info ===== //
    setWindowTitle("SLV (Windows) (dev version)");
    // setWindowIcon(QIcon("../icon/numalyse_logo.ico"));
    // QString path = "../icon/numalyse_logo.ico";
    // QFile f(path);
    // qDebug() << "Fichier existe ?" << f.exists();
    // qDebug() << "Current Working Directory:" << QDir::currentPath();
    // qDebug() << QFile::exists("../icon/numal_logo.ico");

    auto& prefManager = PrefManager::instance();
    prefManager.loadPrefs();
    prefManager.loadLanguage(prefManager.getPref("Interface","Lang","code"));

    auto *rootLayout = new QVBoxLayout(ui->centralwidget);
    rootLayout->setContentsMargins(0,0,0,0);

    showMaximized();

    m_globalPlayerManager = new GlobalPlayerManager(this);

    auto *layout = ui->centralwidget->layout();
    layout->addWidget(m_globalPlayerManager);

    createMenuBar();
    createToolBar();
    statusBar()->hide();

    connect(m_globalPlayerManager, &GlobalPlayerManager::enableFullscreenMainRequested, this, &MainWindow::enableFullscreenMain);
    connect(m_globalPlayerManager, &GlobalPlayerManager::disableFullscreenMainRequested, this, &MainWindow::disableFullscreenMain);
    connect(m_globalPlayerManager, &GlobalPlayerManager::disableNavPanelRequested, this, &MainWindow::disableNavPanel);
    connect(m_globalPlayerManager, &GlobalPlayerManager::enableNavPanelRequested, this, &MainWindow::enableNavPanel);
    connect(m_navPanelBtn, &ToolbarToggleButton::stateActivated, this, [this]{
        m_globalPlayerManager->openNavPanel();
        m_navPanelBtn->setButtonState(true);
    });
    connect(m_navPanelBtn, &ToolbarToggleButton::stateDeactivated, this, [this]{
        m_globalPlayerManager->closeNavPanel();
        m_navPanelBtn->setButtonState(false);
    });

    connect(this, &MainWindow::windowMovedOrResizedRequested, &SignalManager::instance(), [](){ emit SignalManager::instance().windowMovedOrResized(); });
    std::vector<std::pair<ToolbarButton*, PlayerLayoutArrangement>> layoutButtons = {
        {m_view1, Arrangement1},
        {m_view2H, Arrangement2H},
        {m_view2V, Arrangement2V},
        {m_view3HAlign, Arrangement3H},
        {m_view3VAlign, Arrangement3V},
        {m_view3Top, Arrangement3Top},
        {m_view3Bot, Arrangement3Bot},
        {m_view3Left, Arrangement3Left},
        {m_view3Right, Arrangement3Right},
        {m_view4, Arrangement4}
    };

    for (const auto& pair : layoutButtons) {
        ToolbarButton* btn = pair.first;
        PlayerLayoutArrangement arrangement = pair.second;
        connect(btn, &ToolbarButton::clicked, this, [this, arrangement]() { 
            changeArrangementWithSaveCheck(arrangement); 
        });
    }
    
}

void MainWindow::createMenuBar()
{
    auto *projManager = &ProjectManager::instance();
    auto& prefManager = PrefManager::instance();
    auto *fileMenu = menuBar()->addMenu("&" + prefManager.getText("main_window_menu_bar_file"));

    auto *openMediaAction = fileMenu->addAction("&" + prefManager.getText("main_window_file_open_media_action"));
    openMediaAction->setShortcut(QKeySequence(prefManager.getPref("Shortcuts", "MainWindow", "open_media")));

    auto *openProjectAction = fileMenu->addAction("&" + prefManager.getText("main_window_file_open_project_action"));
    openProjectAction->setShortcut(QKeySequence(prefManager.getPref("Shortcuts", "MainWindow", "open_project")));

    auto *saveProjectAction = fileMenu->addAction("&" + prefManager.getText("main_window_file_save_project_action"));
    saveProjectAction->setShortcut(QKeySequence(prefManager.getPref("Shortcuts", "MainWindow", "save_project")));
    saveProjectAction->setDisabled(true);

    connect(projManager, &ProjectManager::enableSaveButton, this, [saveProjectAction](){
        saveProjectAction->setEnabled(true);
    });
    connect(projManager, &ProjectManager::disableSaveButton, this, [saveProjectAction](){
        saveProjectAction->setDisabled(true);
    });
    connect(saveProjectAction, &QAction::triggered, this, [projManager]() {
        projManager->setSaveNeeded();
        projManager->saveProject(false);
    });

    connect(openMediaAction, &QAction::triggered, this, &MainWindow::openMediaAction);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::openProjectAction);

    auto *OptionMenu = menuBar()->addMenu("&" + prefManager.getText("main_window_menu_bar_option"));

    auto *openPrefAction = OptionMenu->addAction("&" + prefManager.getText("main_window_option_open_pref"));
    connect(openPrefAction, &QAction::triggered, this, &MainWindow::openPrefWidget);


    // menuBar()->setCornerWidget(m_navPanelBtn, Qt::TopRightCorner);

}

void MainWindow::createToolBar()
{
    m_toolbarQt = new QToolBar(this);

    createViewGridBtn();

    m_navPanelBtn = new ToolbarToggleButton(
        m_toolbarQt,
        false,
        "nav_panel_menu_open_white",
        PrefManager::instance().getText("tooltip_nav_panel_close"),
        "nav_panel_menu_closed_white",
        PrefManager::instance().getText("tooltip_nav_panel_open")
    );
    m_navPanelBtn->setFixedSize(30, 30);
    m_navPanelBtn->setIconSize(QSize(20, 20));
    m_navPanelBtn->setStyleSheet("border: none;");

    m_toolbarQt->setMovable(false);
    m_toolbarQt->setFloatable(false);
    // toolbar->addSeparator();
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_toolbarQt->addWidget(spacer);
    m_toolbarQt->addWidget(m_viewGridBtn);
    // m_toolbarQt->addWidget(m_view1);
    // m_toolbarQt->addWidget(m_view2);
    // m_toolbarQt->addWidget(m_view3);
    // m_toolbarQt->addWidget(m_view4);
    m_toolbarQt->addWidget(m_navPanelBtn);
    m_toolbarQt->setStyleSheet("border: none;");

    addToolBar(m_toolbarQt);
}

void MainWindow::openProjectAction()
{
    ProjectManager& projManager = ProjectManager::instance();
    PrefManager& txtManager = PrefManager::instance();

    if(projManager.needSave()){
        SLV::showGenericDialog(
            this, 
            txtManager.getText("dialog_save_project_dialog_title"),
            txtManager.getText("dialog_save_project_dialog_text"),
            
            [&projManager]() { 
                projManager.saveProject(false); 
                projManager.openProject();
            },
            
            [&projManager]() { 
                projManager.openProject();
            }
        );
    } else {
        projManager.openProject();
    }

}


void MainWindow::openMediaAction()
{
    ProjectManager& projManager = ProjectManager::instance();
    PrefManager& txtManager = PrefManager::instance();

    if(projManager.projet() && projManager.needSave()){
        SLV::showGenericDialog(
            this, 
            txtManager.getText("dialog_save_project_dialog_title"),
            txtManager.getText("dialog_save_project_dialog_text"),
            
            [this, &projManager]() { 
                projManager.saveProject(false); 
                this->selectAndLoadMediaFiles(); 
            },
            
            [this]() { 
                this->selectAndLoadMediaFiles(); 
            }
        );
    } else {
        selectAndLoadMediaFiles();
    }

}


void MainWindow::selectAndLoadMediaFiles()
{
    auto& prefManager = PrefManager::instance();
    QStringList files_paths = QFileDialog::getOpenFileNames(
        this, 
        prefManager.getText("open_files"), 
        prefManager.getPref("Paths", "lp_open_media"), 
        "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)"
    );
    
    if(files_paths.empty()){
        qDebug() << "Pas de fichier sélectionné";
        return;
    }
    if(files_paths.size() > 4){
        qDebug() << "Trop de fichiers sélectionnés";
        return;
    }

    QFileInfo fileInfo (files_paths[0]);
    prefManager.setPref("Paths", "lp_open_media", fileInfo.absolutePath());
    
    qDebug() << "Fichiers sélectionnés : " << files_paths;
    for(const QString& path : files_paths){
        qDebug() << path;
    }
    
    m_globalPlayerManager->setPlayersFromPaths(files_paths);
}

MainWindow::~MainWindow()
{
    delete ui;

}



void MainWindow::enableFullscreenMain()
{
    wasMaximized = isMaximized();
    ui->menubar->hide();
    m_toolbarQt->hide();
    emit m_navPanelBtn->stateDeactivated();
    showFullScreen();
}

void MainWindow::disableFullscreenMain()
{
    ui->menubar->show();
    m_toolbarQt->show();
    if(!wasMaximized)
        showNormal();
    else
        showMaximized();
}

void MainWindow::disableNavPanel()
{
    emit m_navPanelBtn->stateDeactivated();
    m_navPanelBtn->blockSignals(true);
}

void MainWindow::enableNavPanel()
{
    m_navPanelBtn->blockSignals(false);
}

void MainWindow::createViewGridBtn()
{
    QHBoxLayout *viewLayout = new QHBoxLayout();

    m_view1 = new ToolbarButton(nullptr, "view_1_white", PrefManager::instance().getText("tooltip_view_1"));
    viewLayout->addWidget(m_view1);

    QHBoxLayout *view2Layout = new QHBoxLayout();
    m_view2H = new ToolbarButton(nullptr, "view_2_h_white", PrefManager::instance().getText("tooltip_view_2_h"));
    m_view2V = new ToolbarButton(nullptr, "view_2_v_white", PrefManager::instance().getText("tooltip_view_2_v"));
    view2Layout->addWidget(m_view2H);
    view2Layout->addWidget(m_view2V);
    m_view2 = new ToolbarToggleHoverButton(nullptr, view2Layout, false, "view_2_h_white", PrefManager::instance().getText("tooltip_view_2"), "view_2_h_white", PrefManager::instance().getText("tooltip_view_2"));
    m_view2->setOnTop(false);
    m_view2->setToolTip("");
    viewLayout->addWidget(m_view2);

    QGridLayout *view3Layout = new QGridLayout();
    m_view3HAlign = new ToolbarButton(nullptr, "view_3_h_white", PrefManager::instance().getText("tooltip_view_3_h"));
    m_view3VAlign = new ToolbarButton(nullptr, "view_3_v_white", PrefManager::instance().getText("tooltip_view_3_v"));
    m_view3Bot = new ToolbarButton(nullptr, "view_3_bot_white", PrefManager::instance().getText("tooltip_view_3_bot"));
    m_view3Top = new ToolbarButton(nullptr, "view_3_top_white", PrefManager::instance().getText("tooltip_view_3_top"));
    m_view3Left = new ToolbarButton(nullptr, "view_3_left_white", PrefManager::instance().getText("tooltip_view_3_left"));
    m_view3Right = new ToolbarButton(nullptr, "view_3_right_white", PrefManager::instance().getText("tooltip_view_3_right"));
    view3Layout->addWidget(m_view3HAlign, 0, 0);
    view3Layout->addWidget(m_view3VAlign, 1, 0);
    view3Layout->addWidget(m_view3Bot, 0, 1);
    view3Layout->addWidget(m_view3Top, 1, 1);
    view3Layout->addWidget(m_view3Left, 0, 2);
    view3Layout->addWidget(m_view3Right, 1, 2);
    m_view3 = new ToolbarToggleHoverButton(nullptr, view3Layout, false, "view_3_bot_white", PrefManager::instance().getText("tooltip_view_3"), "view_3_bot_white", PrefManager::instance().getText("tooltip_view_3"));
    m_view3->setOnTop(false);
    m_view3->setToolTip("");
    viewLayout->addWidget(m_view3);

    m_view4 = new ToolbarButton(nullptr, "view_4_white", PrefManager::instance().getText("tooltip_view_4"));
    viewLayout->addWidget(m_view4);

    m_viewGridBtn = new ToolbarToggleHoverButton(m_toolbarQt, viewLayout, false, "player_arrangement_white", PrefManager::instance().getText("tooltip_view_grid"), "player_arrangement_white"),  PrefManager::instance().getText("tooltip_view_grid");
    m_viewGridBtn->setOnTop(false);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    emit windowMovedOrResizedRequested();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    emit windowMovedOrResizedRequested();
}

void MainWindow::changeArrangementWithSaveCheck(PlayerLayoutArrangement arrangement)
{
    ProjectManager& projManager = ProjectManager::instance();
    
    if (projManager.needSave()) { 
        PrefManager& txtManager = PrefManager::instance();
        
        SLV::showGenericDialog(
            this, 
            txtManager.getText("dialog_save_project_dialog_title"),
            txtManager.getText("dialog_save_project_dialog_text"),
            [&projManager, arrangement]() { 
                projManager.saveProject(false); 
                emit SignalManager::instance().newArrangementRequested(arrangement);
            },
            [arrangement]() { 
                emit SignalManager::instance().newArrangementRequested(arrangement);
            }
        );
    } else {
        emit SignalManager::instance().newArrangementRequested(arrangement);
    }
}

void MainWindow::openPrefWidget()
{
    qDebug() << "open pref widget";
    PreferenceDialog* prefDialog = new PreferenceDialog(this);
    prefDialog->exec();
}
