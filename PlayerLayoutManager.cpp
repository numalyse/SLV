#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"
#include "Toolbars/Toolbar.h"
#include "Toolbars/GlobalToolbar.h"
#include "Toolbars/AdvancedToolbar.h"

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

    m_players.reserve(s_maxPlayerCount);
    for (size_t IPlayer = 0; IPlayer < s_maxPlayerCount; IPlayer++){
        PlayerWidget* player = new PlayerWidget(this);
        connect(player, &PlayerWidget::addPlayerRequest, this, &PlayerLayoutManager::addPlayer);
        connect(player, &PlayerWidget::removePlayerRequest, this, &PlayerLayoutManager::removePlayer);
        connect(player, &PlayerWidget::enablePlayerFullscreenRequested, this, &PlayerLayoutManager::enableLayoutFullscreen);
        connect(player, &PlayerWidget::disablePlayerFullscreenRequested, this, &PlayerLayoutManager::disableLayoutFullscreen);
        m_players.append(player);
    }
    
}

PlayerLayoutManager::~PlayerLayoutManager()
{
    for (size_t IPlayer = 0; IPlayer < m_players.size(); IPlayer++)
    {
        PlayerWidget* player = new PlayerWidget(this);
        connect(player, &PlayerWidget::removePlayerRequest, this, &PlayerLayoutManager::removePlayer);
        m_players.append(player);
    }
}


void PlayerLayoutManager::activePlayerUpdate(const int activePlayersNeeded){
    int activePlayerCount = m_activePlayers.size();
    
    if(activePlayersNeeded < activePlayerCount){

        m_activePlayers.erase(m_activePlayers.begin() + activePlayersNeeded, m_activePlayers.end());

    }else if(activePlayersNeeded > activePlayerCount){ 

        for( auto & IPlayer : m_players){
            if(!m_activePlayers.contains(IPlayer)){
                m_activePlayers.append(IPlayer);
                ++activePlayerCount;
                if(activePlayersNeeded == activePlayerCount){
                    return;
                }
            }
        }
    }
}

void PlayerLayoutManager::createLayout(const int count)
{
     // les players vont etre détruit si on delete l'ancien widget, il faut d'abord modifier leurs parents
    detachAllPlayers();
    activePlayerUpdate(count);

    QWidget* container = nullptr;

    switch (count){
        case 1: 
            container = create1();
            break;
        case 2: 
            container = create2();
            break;
        case 3: 
            container = create3();
            break;
        case 4: 
            container = create4();
            break;

        default: container = nullptr;
    }
    auto* toolbar = createLayoutToolbar();
    emit updateContainerRequest(m_activePlayers.size(), container, toolbar);
}

void PlayerLayoutManager::createLayoutFromPaths(const QStringList& filesPaths)
{
    detachAllPlayers();

    int pathCount = filesPaths.size();
    activePlayerUpdate(pathCount);

    QWidget* container = nullptr;

    switch (pathCount){
        case 1: 
            container = create1(filesPaths);
            break;
        case 2: 
            container = create2(filesPaths);
            break;
        case 3: 
            container = create3(filesPaths);
            break;
        case 4: 
            container = create4(filesPaths);
            break;

        default: container = nullptr;
    }
    auto* toolbar = createLayoutToolbar();
    emit updateContainerRequest(m_activePlayers.size(), container, toolbar);

}

void PlayerLayoutManager::detachAllPlayers()
{
    for(auto& IPlayer : m_players){
        IPlayer->setParent(this);
    }
}

QWidget* PlayerLayoutManager::create1(const QStringList& filePath)
{
    Q_ASSERT(m_activePlayers.size() == 1);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);

    if (!m_activePlayers.isEmpty()){
        if(filePath != QStringList(""))
            m_activePlayers[0]->setMediaFromPath(filePath.at(0));
        layout->addWidget(m_activePlayers[0]);
    }

    qDebug() << "Media set";

    return container;
}

QWidget* PlayerLayoutManager::create2(const QStringList& filesPaths)
{
    Q_ASSERT(m_activePlayers.size() == 2);

    auto *splitter = new QSplitter(Qt::Horizontal);

    if (m_activePlayers.size() >= 2) {
        if(filesPaths != QStringList(""))
            for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
                m_activePlayers[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
            }
        splitter->addWidget(m_activePlayers[0]);
        splitter->addWidget(m_activePlayers[1]);
    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);

    return container;
}

QWidget* PlayerLayoutManager::create3(const QStringList& filesPaths)
{
    Q_ASSERT(m_activePlayers.size() == 3);

    if (m_activePlayers.size() < 3) return nullptr;

    if(filesPaths != QStringList(""))
        for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
            m_activePlayers[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
        }

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    top->addWidget(m_activePlayers[0]);
    top->addWidget(m_activePlayers[1]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(m_activePlayers[2]);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}

QWidget* PlayerLayoutManager::create4(const QStringList& filesPaths)
{
    Q_ASSERT(m_activePlayers.size() == 4);

    if (m_activePlayers.size() < 4) return nullptr;

    // S'il y a des paths d'un média en paramètre, charge les médias dans l'ordre
    if(filesPaths != QStringList(""))
        for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
            m_activePlayers[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
        }

    auto *mainSplitter = new QSplitter(Qt::Vertical);

    auto *top = new QSplitter(Qt::Horizontal);
    auto *bottom = new QSplitter(Qt::Horizontal);

    top->addWidget(m_activePlayers[0]);
    top->addWidget(m_activePlayers[1]);

    bottom->addWidget(m_activePlayers[2]);
    bottom->addWidget(m_activePlayers[3]);

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(bottom);

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}

Toolbar* PlayerLayoutManager::createGlobalToolbar(){
    GlobalToolbar* globalToolbar = new GlobalToolbar(nullptr);

    for(auto& IActivePlayer : m_activePlayers){
        IActivePlayer->getToolbar()->show();
        connect(globalToolbar, &GlobalToolbar::playRequest, IActivePlayer, &PlayerWidget::play);
        connect(globalToolbar, &GlobalToolbar::pauseRequest, IActivePlayer, &PlayerWidget::pause);
        connect(globalToolbar, &GlobalToolbar::stopRequest, IActivePlayer, &PlayerWidget::stop);
        connect(globalToolbar, &GlobalToolbar::ejectRequest, IActivePlayer, &PlayerWidget::eject);
    }

    return static_cast<Toolbar*>(globalToolbar);
}

Toolbar* PlayerLayoutManager::createAdvancedToolbar(){
    Q_ASSERT(m_activePlayers.size() == 1);

    auto* activePlayer = m_activePlayers[0];
    if( activePlayer ){
        activePlayer->getToolbar()->hide();
    }

    AdvancedToolbar* advancedToolbar = new AdvancedToolbar(nullptr);

    connect(advancedToolbar, &AdvancedToolbar::playRequest, activePlayer, &PlayerWidget::play);
    connect(advancedToolbar, &AdvancedToolbar::pauseRequest, activePlayer, &PlayerWidget::pause);
    connect(advancedToolbar, &AdvancedToolbar::stopRequest, activePlayer, &PlayerWidget::stop);
    connect(advancedToolbar, &AdvancedToolbar::ejectRequest, activePlayer, &PlayerWidget::eject);
    connect(advancedToolbar, &AdvancedToolbar::enableMuteRequest, activePlayer, &PlayerWidget::mute);
    connect(advancedToolbar, &AdvancedToolbar::disableMuteRequest, activePlayer, &PlayerWidget::unmute);
    connect(advancedToolbar, &AdvancedToolbar::volumeChanged, activePlayer, &PlayerWidget::setVolume);
    connect(advancedToolbar, &AdvancedToolbar::speedChanged, activePlayer, &PlayerWidget::setSpeed);
    connect(advancedToolbar, &AdvancedToolbar::screenshotRequest, activePlayer, &PlayerWidget::takeScreenshot);
    connect(advancedToolbar, &AdvancedToolbar::setPositionRequested, activePlayer, &PlayerWidget::setTime);

    connect(activePlayer, &PlayerWidget::updateSliderRangeRequest, advancedToolbar, &AdvancedToolbar::updateSliderRange);
    connect(activePlayer, &PlayerWidget::updateSliderValueRequest, advancedToolbar, &AdvancedToolbar::updateSliderValue);
    connect(activePlayer, &PlayerWidget::updateFpsRequest, advancedToolbar, &AdvancedToolbar::updateFps);

    return static_cast<Toolbar*>(advancedToolbar);
}

Toolbar *PlayerLayoutManager::createLayoutToolbar()
{
    if(m_activePlayers.size() == 1 ){ 
        return createAdvancedToolbar();
    }else {
        return createGlobalToolbar();
    }
}

// slots
void PlayerLayoutManager::addPlayer()
{
    int activePlayerCount = m_activePlayers.size();
    if(activePlayerCount < 4){
        createLayout(activePlayerCount + 1 );
    }
}

void PlayerLayoutManager::removePlayer(PlayerWidget* playerToRemove){
    int activePlayerCount = m_activePlayers.size();
    if (activePlayerCount > 1){
        m_activePlayers.removeOne(playerToRemove);
        createLayout(m_activePlayers.size());
    }
}

void PlayerLayoutManager::enableLayoutFullscreen(PlayerWidget* playerToFullscreen){
    for(auto &IPlayer : m_players){
        if(playerToFullscreen != IPlayer)
            IPlayer->hide();
    }
    emit enableFullscreenGlobalRequested();
}

void PlayerLayoutManager::disableLayoutFullscreen(PlayerWidget* playerToFullscreen){
    for(auto &IPlayer : m_players){
        IPlayer->show();
    }
    emit disableFullscreenGlobalRequested();
}
