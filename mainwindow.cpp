#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PlayerLayoutManager.h"
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

    m_layoutManager = new PlayerLayoutManager(this);

    for (int i = 0; i < 4; ++i) {
        auto *player = new PlayerWidget;
        m_players.append(player);
    }

    setPlayers(1);

}

MainWindow::~MainWindow()
{
    m_players.clear();
    delete ui;

}



void MainWindow::setPlayers(int count)
{
    auto *layout = ui->centralwidget->layout();

    if (m_currentLayout) {
        layout->removeWidget(m_currentLayout);
        delete m_currentLayout;
        m_currentLayout = nullptr;
    }

    m_currentLayout = m_layoutManager->createLayout(m_players, count);
    if (m_currentLayout)
        layout->addWidget(m_currentLayout);
}




