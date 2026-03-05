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
    TextManager::instance().loadLanguage("en");

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
    connect(m_navPanelBtn, &ToolbarToggleButton::stateActivated, m_globalPlayerManager, &GlobalPlayerManager::openNavPanel);
    connect(m_navPanelBtn, &ToolbarToggleButton::stateDeactivated, m_globalPlayerManager, &GlobalPlayerManager::closeNavPanel);

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

    m_navPanelBtn = new ToolbarToggleButton(
        menuBar(),
        false,
        "nav_panel_menu_open.png",
        TextManager::instance().get("tooltip_nav_panel_open"),
        "nav_panel_menu_closed.png",
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
    ui->menubar->hide();
    m_toolbarQt->hide();
    showFullScreen();
}

void MainWindow::disableFullscreenMain()
{
    ui->menubar->show();
    m_toolbarQt->show();
    showNormal();
}
