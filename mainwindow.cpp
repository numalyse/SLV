#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "GlobalPlayerManager.h"
#include "PlayerWidget.h"

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



    auto *rootLayout = new QVBoxLayout(ui->centralwidget);
    rootLayout->setContentsMargins(0,0,0,0);

    m_globalPlayerManager = new GlobalPlayerManager(this);

    auto *layout = ui->centralwidget->layout();
    layout->addWidget(m_globalPlayerManager);
}

MainWindow::~MainWindow()
{
    delete ui;

}








