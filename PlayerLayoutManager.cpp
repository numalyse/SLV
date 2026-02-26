#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"

#include <QObject>
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>
#include <QtAssert>

// #include <iostream>

PlayerLayoutManager::PlayerLayoutManager(QObject *parent)
    : QWidget{}
{
    m_players.reserve(4);
    PlayerWidget* player = new PlayerWidget(this);
    PlayerWidget* player2 = new PlayerWidget(this);
    PlayerWidget* player3 = new PlayerWidget(this);
    PlayerWidget* player4 = new PlayerWidget(this);
    m_players.append(player);
    m_players.append(player2);
    m_players.append(player3);
    m_players.append(player4);
    connect(player, &PlayerWidget::addPlayer, this, &PlayerLayoutManager::addPlayer2);
    connect(player, &PlayerWidget::removePlayer, this, &PlayerLayoutManager::removePlayer2);
}

PlayerLayoutManager::~PlayerLayoutManager()
{
    for (size_t IPlayer = 0; IPlayer < m_players.size(); IPlayer++)
    {
        // fuite mÃ©moire ? peut etre ?
    }

}


QWidget* PlayerLayoutManager::createLayout(const int count)
{
    switch (count){
    case 1: return create1();
    case 2: return create2();
    case 3: return create3();
    case 4: return create4();
    default: return nullptr;
    }
}

QWidget* PlayerLayoutManager::createLayoutFromPaths(const QStringList& filesPaths)
{
    switch (filesPaths.size()){
    case 1: return create1(filesPaths);
    case 2: return create2(filesPaths);
    case 3: return create3(filesPaths);
    case 4: return create4(filesPaths);
    default: return nullptr;
    }
}

QWidget* PlayerLayoutManager::create1(const QStringList& filePath)
{
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);

    if (!m_players.isEmpty()){
        if(filePath != QStringList(""))
            m_players[0]->setMediaFromPath(filePath.at(0));
        layout->addWidget(m_players[0]);
    }

    qDebug() << "Media set";

    return container;
}

QWidget* PlayerLayoutManager::create2(const QStringList& filesPaths)
{
    auto *splitter = new QSplitter(Qt::Horizontal);

    if (m_players.size() >= 2) {
        if(filesPaths != QStringList(""))
            for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
                m_players[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
            }
        splitter->addWidget(m_players[0]);
        splitter->addWidget(m_players[1]);
    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);

    return container;
}

QWidget* PlayerLayoutManager::create3(const QStringList& filesPaths)
{
    if (m_players.size() < 3) return nullptr;

    if(filesPaths != QStringList(""))
        for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
            m_players[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
        }

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    top->addWidget(m_players[0]);
    top->addWidget(m_players[1]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(m_players[2]);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}

QWidget* PlayerLayoutManager::create4(const QStringList& filesPaths)
{
    if (m_players.size() < 4) return nullptr;

    // S'il y a des paths d'un média en paramètre, charge les médias dans l'ordre
    if(filesPaths != QStringList(""))
        for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
            m_players[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
        }

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    auto *bottom = new QSplitter(Qt::Horizontal);

    top->addWidget(m_players[0]);
    top->addWidget(m_players[1]);

    bottom->addWidget(m_players[2]);
    bottom->addWidget(m_players[3]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(bottom);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}


// slots
void PlayerLayoutManager::addPlayer2()
{
    if(m_players.size() < 4){
        auto* player = new PlayerWidget();
        m_players.append(player);
        connect(player, &PlayerWidget::addPlayer, this, &PlayerLayoutManager::addPlayer2);
        connect(player, &PlayerWidget::removePlayer, this, &PlayerLayoutManager::removePlayer2);
        auto* container = createLayout(m_players.size());
        emit updateContainer(container);
    }
}

void PlayerLayoutManager::removePlayer2(PlayerWidget* playerToRemove){
    if (m_players.size() > 1){
        m_players.removeOne(playerToRemove);
        auto* container = createLayout(m_players.size());
        emit updateContainer(container);
    }
}
