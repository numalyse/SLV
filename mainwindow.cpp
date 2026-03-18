#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "GlobalPlayerManager.h"
#include "PlayerWidget.h"
#include "TextManager.h"

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

    // TODO : stocker la langue que l'utilisateur choisie dans le preference manager ect.
    TextManager::instance().loadLanguage("fr");

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
    connect(m_view1, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement1); });
    connect(m_view2H, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement2H); });
    connect(m_view2V, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement2V); });
    connect(m_view3HAlign, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3H); });
    connect(m_view3VAlign, &ToolbarButton::clicked, &SignalManager::instance(),[](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3V); });
    connect(m_view3Top, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3Top); });
    connect(m_view3Bot, &ToolbarButton::clicked, &SignalManager::instance(),[](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3Bot); });
    connect(m_view3Left, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3Left); });
    connect(m_view3Right, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement3Right); });
    connect(m_view4, &ToolbarButton::clicked, &SignalManager::instance(), [](){ emit SignalManager::instance().newArrangementRequested(PlayerLayoutArrangement::Arrangement4); });

}

void MainWindow::createMenuBar()
{
    auto *fileMenu = menuBar()->addMenu("&Fichier");
    auto *openMediaAction = fileMenu->addAction("&Ouvrir des fichiers multimédia");
    connect(openMediaAction, &QAction::triggered, this, &MainWindow::openMediaFile);


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
        TextManager::instance().get("tooltip_nav_panel_open"),
        "nav_panel_menu_closed_white",
        TextManager::instance().get("tooltip_nav_panel_close")
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

void MainWindow::openMediaFile()
{
    QStringList files_paths = QFileDialog::getOpenFileNames(this, "Ouvrir des fichiers multimédia", "/", "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)");
    if(files_paths.empty()){
        qDebug() << "Pas de fichier sélectionné";
        return;
    }
    if(files_paths.size() > 4){
        qDebug() << "Trop de fichiers sélectionnés";
        return;
    }
    qDebug() << "Fichiers sélectionnés : " << files_paths;
    for(size_t IFilePath = 0; IFilePath < files_paths.size(); ++IFilePath){
        qDebug() << files_paths.at(IFilePath);
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

    m_view1 = new ToolbarButton(nullptr, "view_1_white", TextManager::instance().get("tooltip_view_1"));
    viewLayout->addWidget(m_view1);

    QHBoxLayout *view2Layout = new QHBoxLayout();
    m_view2H = new ToolbarButton(nullptr, "view_2_h_white", TextManager::instance().get("tooltip_view_2_h"));
    m_view2V = new ToolbarButton(nullptr, "view_2_v_white", TextManager::instance().get("tooltip_view_2_v"));
    view2Layout->addWidget(m_view2H);
    view2Layout->addWidget(m_view2V);
    m_view2 = new ToolbarToggleHoverButton(nullptr, view2Layout, false, "view_2_h_white", TextManager::instance().get("tooltip_view_2"), "view_2_h_white", TextManager::instance().get("tooltip_view_2"));
    m_view2->setOnTop(false);
    m_view2->setToolTip("");
    viewLayout->addWidget(m_view2);

    QGridLayout *view3Layout = new QGridLayout();
    m_view3HAlign = new ToolbarButton(nullptr, "view_3_h_white", TextManager::instance().get("tooltip_view_3_h"));
    m_view3VAlign = new ToolbarButton(nullptr, "view_3_v_white", TextManager::instance().get("tooltip_view_3_v"));
    m_view3Bot = new ToolbarButton(nullptr, "view_3_bot_white", TextManager::instance().get("tooltip_view_3_bot"));
    m_view3Top = new ToolbarButton(nullptr, "view_3_top_white", TextManager::instance().get("tooltip_view_3_top"));
    m_view3Left = new ToolbarButton(nullptr, "view_3_left_white", TextManager::instance().get("tooltip_view_3_left"));
    m_view3Right = new ToolbarButton(nullptr, "view_3_right_white", TextManager::instance().get("tooltip_view_3_right"));
    view3Layout->addWidget(m_view3HAlign, 0, 0);
    view3Layout->addWidget(m_view3VAlign, 1, 0);
    view3Layout->addWidget(m_view3Bot, 0, 1);
    view3Layout->addWidget(m_view3Top, 1, 1);
    view3Layout->addWidget(m_view3Left, 0, 2);
    view3Layout->addWidget(m_view3Right, 1, 2);
    m_view3 = new ToolbarToggleHoverButton(nullptr, view3Layout, false, "view_3_bot_white", TextManager::instance().get("tooltip_view_3"), "view_3_bot_white", TextManager::instance().get("tooltip_view_3"));
    m_view3->setOnTop(false);
    m_view3->setToolTip("");
    viewLayout->addWidget(m_view3);

    m_view4 = new ToolbarButton(nullptr, "view_4_white", TextManager::instance().get("tooltip_view_4"));
    viewLayout->addWidget(m_view4);

    m_viewGridBtn = new ToolbarToggleHoverButton(m_toolbarQt, viewLayout, false, "player_arrangement_white", TextManager::instance().get("tooltip_view_grid"), "player_arrangement_white"),  TextManager::instance().get("tooltip_view_grid");
    m_viewGridBtn->setOnTop(false);
}
