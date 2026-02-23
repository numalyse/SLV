#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"

#include <QObject>
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>

PlayerLayoutManager::PlayerLayoutManager(QObject *parent)
    : QObject{parent}
{

}


QWidget* PlayerLayoutManager::createLayout(const QVector<PlayerWidget*> &players, int count)
{
    switch (count){
    case 1: return create1(players);
    case 2: return create2(players);
    case 3: return create3(players);
    case 4: return create4(players);
        default: return nullptr;
    }
}

QWidget* PlayerLayoutManager::create1(const QVector<PlayerWidget*>& players)
{
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);

    if (!players.isEmpty())
        layout->addWidget(players[0]);

    return container;
}

QWidget* PlayerLayoutManager::create2(const QVector<PlayerWidget*>& players)
{
    auto *splitter = new QSplitter(Qt::Horizontal);

    if (players.size() >= 2) {
        splitter->addWidget(players[0]);
        splitter->addWidget(players[1]);
    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);

    return container;
}

QWidget* PlayerLayoutManager::create3(const QVector<PlayerWidget*>& players)
{
    if (players.size() < 3) return nullptr;

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    top->addWidget(players[0]);
    top->addWidget(players[1]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(players[2]);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}

QWidget* PlayerLayoutManager::create4(const QVector<PlayerWidget*>& players)
{
    if (players.size() < 4) return nullptr;

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    auto *bottom = new QSplitter(Qt::Horizontal);

    top->addWidget(players[0]);
    top->addWidget(players[1]);

    bottom->addWidget(players[2]);
    bottom->addWidget(players[3]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(bottom);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}
