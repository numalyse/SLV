#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"

#include <QObject>
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>

// #include <iostream>

PlayerLayoutManager::PlayerLayoutManager(QObject *parent)
    : QWidget{}
{
    m_players.reserve(4);
    PlayerWidget* player = new PlayerWidget();
    PlayerWidget* player2 = new PlayerWidget();
    m_players.append(player);
    m_players.append(player2);
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


QWidget* PlayerLayoutManager::createLayout()
{
    switch (m_players.size()){
    case 1: return create1();
    case 2: return create2();
    case 3: return create3();
    case 4: return create4();
    default: return nullptr;
    }
}

QWidget* PlayerLayoutManager::create1()
{
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);

    if (!m_players.isEmpty())
        layout->addWidget(m_players[0]);

    return container;
}

QWidget* PlayerLayoutManager::create2()
{
    auto *splitter = new QSplitter(Qt::Horizontal);

    if (m_players.size() >= 2) {
        splitter->addWidget(m_players[0]);
        splitter->addWidget(m_players[1]);
    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);

    return container;
}

QWidget* PlayerLayoutManager::create3()
{
    if (m_players.size() < 3) return nullptr;

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

QWidget* PlayerLayoutManager::create4()
{
    if (m_players.size() < 4) return nullptr;

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
        auto* container = createLayout();
        emit updateContainer(container);
    }
}

void PlayerLayoutManager::removePlayer2(PlayerWidget* playerToRemove){
    if (m_players.size() > 1){
        m_players.removeOne(playerToRemove);
        auto* container = createLayout();
        emit updateContainer(container);
    }
}
