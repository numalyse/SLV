#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"
#include "Toolbars/Toolbar.h"
#include "Toolbars/GlobalToolbar.h"
#include "Toolbars/AdvancedToolbar.h"
#include "ProjectManager.h"

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
    connect(&SignalManager::instance(), &SignalManager::newArrangementRequested, this, &PlayerLayoutManager::arrangePlayerLayout);
    for (size_t IPlayer = 0; IPlayer < s_maxPlayerCount; IPlayer++){
        PlayerWidget* player = new PlayerWidget(this);
        connect(player, &PlayerWidget::duplicatePlayerRequest, this, &PlayerLayoutManager::duplicatePlayer);
        connect(player, &PlayerWidget::removePlayerRequest, this, &PlayerLayoutManager::removePlayer);
        connect(player, &PlayerWidget::enablePlayerFullscreenRequested, this, &PlayerLayoutManager::enablePlayerLayoutFullscreen);
        connect(player, &PlayerWidget::disablePlayerFullscreenRequested, this, &PlayerLayoutManager::disablePlayerLayoutFullscreen);
        connect(player, &PlayerWidget::checkPlayersPlayStatusRequested, this, &PlayerLayoutManager::checkPlayersPlayStatus);
        connect(player, &PlayerWidget::checkPlayersMuteStatusRequested, this, &PlayerLayoutManager::checkPlayersMuteStatus);
        connect(&SignalManager::instance(), &SignalManager::playerWidgetMediaDropped, this, &PlayerLayoutManager::createLayoutFromPaths);
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

        for(int IActivePlayer = activePlayersNeeded; IActivePlayer < activePlayerCount; ++IActivePlayer){
            m_activePlayers[IActivePlayer]->eject();
        }
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

void PlayerLayoutManager::createLayout(const int count, const PlayerLayoutArrangement& arrangement)
{
     // les players vont etre détruit si on delete l'ancien widget, il faut d'abord modifier leurs parents
    detachAllPlayers();
    activePlayerUpdate(count);

    QWidget* container = nullptr;
    PlayerWidget* player = nullptr;
    switch (count){
        case 1: 
            container = create1();
            player = m_activePlayers[0];
            emit enableNavPanelRequested();
            break;
        case 2:
            {
                Qt::Orientation orientation = arrangement == PlayerLayoutArrangement::Arrangement2H ? Qt::Horizontal : Qt::Vertical;
                container = create2(QStringList(), orientation);
                emit disableNavPanelRequested();
            }
            break;
        case 3:
            container = create3(QStringList(), arrangement);
            emit disableNavPanelRequested();
            break;
        case 4: 
            container = create4();
            emit disableNavPanelRequested();
            break;

        default: container = nullptr;
    }
    auto* toolbar = createLayoutToolbar();
    ProjectManager::instance().requestProjectCreation(getActivePlayersMediaPath());
    emit updateContainerRequest(player, container, toolbar);
}

void PlayerLayoutManager::createLayoutFromPaths(const QStringList& filesPaths)
{
    detachAllPlayers();

    int pathCount = filesPaths.size();
    activePlayerUpdate(pathCount);

    QWidget* container = nullptr;
    PlayerWidget* player = nullptr;
    switch (pathCount){
        case 1: 
            container = create1(filesPaths);
            player = m_activePlayers[0];
            emit enableNavPanelRequested();
            break;
        case 2: 
            container = create2(filesPaths);
            emit disableNavPanelRequested();
            break;
        case 3: 
            container = create3(filesPaths);
            emit disableNavPanelRequested();
            break;
        case 4: 
            container = create4(filesPaths);
            emit disableNavPanelRequested();
            break;

        default: container = nullptr;
    }
    auto* toolbar = createLayoutToolbar();
    ProjectManager::instance().requestProjectCreation(getActivePlayersMediaPath());
    emit updateContainerRequest(player, container, toolbar);

}

/// @brief Fonction appelé par le project manager quand on charge un projet.
/// ProjectManager::instance().requestProjectCreation(getActivePlayersMediaPath()); aurait reset le projet créer 
/// @param filesPaths 
void PlayerLayoutManager::createLayoutFromProject(const QStringList& filesPaths){
    QWidget* container = create1(filesPaths);
    PlayerWidget* player = m_activePlayers[0];
    emit enableNavPanelRequested();

    auto* toolbar = createLayoutToolbar();
    emit updateContainerRequest(player, container, toolbar);
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

QWidget* PlayerLayoutManager::create2(const QStringList& filesPaths, const Qt::Orientation& orientation)
{
    Q_ASSERT(m_activePlayers.size() == 2);

    auto *splitter = new QSplitter(orientation);

    if (m_activePlayers.size() >= 2) {
        if(filesPaths != QStringList(""))
            for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
                m_activePlayers[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
            }
        splitter->addWidget(m_activePlayers[0]);
        splitter->addWidget(m_activePlayers[1]);
        splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);

    return container;
}

QWidget* PlayerLayoutManager::create3(const QStringList& filesPaths, const PlayerLayoutArrangement& arrangement)
{
    Q_ASSERT(m_activePlayers.size() == 3);

    if (m_activePlayers.size() < 3) return nullptr;

    if(filesPaths != QStringList(""))
        for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
            m_activePlayers[IFilePath]->setMediaFromPath(filesPaths.at(IFilePath));
        }

    auto *mainSplitter = new QSplitter();
    if(arrangement == PlayerLayoutArrangement::Arrangement3Bot || arrangement == PlayerLayoutArrangement::Arrangement3Top || arrangement == PlayerLayoutArrangement::Arrangement3V){

        mainSplitter->setOrientation(Qt::Vertical);

        auto *adjacentPlayers = new QSplitter(Qt::Horizontal);
        if(arrangement == PlayerLayoutArrangement::Arrangement3V){
            adjacentPlayers->setOrientation(Qt::Vertical);
            // m_activePlayers[0]->sizePolicy().setHorizontalStretch(1);
            // adjacentPlayers->sizePolicy().setHorizontalStretch(2);
        }
        if(arrangement == PlayerLayoutArrangement::Arrangement3Bot){
            adjacentPlayers->addWidget(m_activePlayers[0]);
            adjacentPlayers->addWidget(m_activePlayers[1]);
            mainSplitter->addWidget(adjacentPlayers);
            mainSplitter->addWidget(m_activePlayers[2]);
        }
        else{
            adjacentPlayers->addWidget(m_activePlayers[1]);
            adjacentPlayers->addWidget(m_activePlayers[2]);
            mainSplitter->addWidget(m_activePlayers[0]);
            mainSplitter->addWidget(adjacentPlayers);
        }

        adjacentPlayers->setSizes(QList<int>({INT_MAX, INT_MAX}));

    }
    else{

        mainSplitter->setOrientation(Qt::Horizontal);

        auto *adjacentPlayers = new QSplitter(Qt::Vertical);
        if(arrangement == PlayerLayoutArrangement::Arrangement3H){
            adjacentPlayers->setOrientation(Qt::Horizontal);

            // m_activePlayers[1]->sizePolicy().setHorizontalStretch(1);
            // adjacentPlayers->sizePolicy().setHorizontalStretch(2);

        }
        if(arrangement == PlayerLayoutArrangement::Arrangement3Right){
            adjacentPlayers->addWidget(m_activePlayers[0]);
            adjacentPlayers->addWidget(m_activePlayers[2]);
            mainSplitter->addWidget(adjacentPlayers);
            mainSplitter->addWidget(m_activePlayers[1]);
        }
        else{
            adjacentPlayers->addWidget(m_activePlayers[1]);
            adjacentPlayers->addWidget(m_activePlayers[2]);
            mainSplitter->addWidget(m_activePlayers[0]);
            mainSplitter->addWidget(adjacentPlayers);
        }

        adjacentPlayers->setSizes(QList<int>({INT_MAX, INT_MAX}));
    }

    mainSplitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
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
    top->setSizes(QList<int>({INT_MAX, INT_MAX}));

    bottom->addWidget(m_activePlayers[2]);
    bottom->addWidget(m_activePlayers[3]);
    bottom->setSizes(QList<int>({INT_MAX, INT_MAX}));

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(bottom);
    mainSplitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);

    return container;
}

Toolbar* PlayerLayoutManager::createGlobalToolbar(){
    GlobalToolbar* globalToolbar = new GlobalToolbar(nullptr);

    // Parcours les players pour connecter le play / play de la global a ses players
    for(auto& IActivePlayer : m_activePlayers){
        IActivePlayer->toolbar()->show();

        connect(globalToolbar, &GlobalToolbar::playRequest, IActivePlayer, &PlayerWidget::play);
        connect(globalToolbar, &GlobalToolbar::pauseRequest, IActivePlayer, &PlayerWidget::pause);
        connect(globalToolbar, &GlobalToolbar::stopRequest, IActivePlayer, &PlayerWidget::stop);
        connect(globalToolbar, &GlobalToolbar::ejectRequest, IActivePlayer, &PlayerWidget::eject);
        connect(globalToolbar, &GlobalToolbar::enableMute, IActivePlayer, &PlayerWidget::mute);
        connect(globalToolbar, &GlobalToolbar::disableMute, IActivePlayer, &PlayerWidget::unmute);
        connect(globalToolbar, &Toolbar::enableFullscreenRequest, this, &PlayerLayoutManager::enableGlobalLayoutFullscreen);
        connect(globalToolbar, &Toolbar::disableFullscreenRequest, this, &PlayerLayoutManager::disableGlobalLayoutFullscreen);
        connect(IActivePlayer, &PlayerWidget::mediaPlayerLoaded, globalToolbar, &GlobalToolbar::enableButtons);
        connect(IActivePlayer, &PlayerWidget::mediaPlayerEjected, this, &PlayerLayoutManager::disableGlobalToolbarButtons);
        connect(this, &PlayerLayoutManager::buttonsDisabled, globalToolbar, &GlobalToolbar::disableButtons);

    }

    globalToolbar->muteBtn()->setButtonState(newGlobalMuteState());
    globalToolbar->playPauseBtn()->setButtonState(newGlobalPlayState());

    return static_cast<Toolbar*>(globalToolbar);
}

Toolbar* PlayerLayoutManager::createAdvancedToolbar(){
    Q_ASSERT(m_activePlayers.size() == 1);

    auto* activePlayer = m_activePlayers[0];
    auto* activePlayerToolbar = activePlayer->toolbar();

    AdvancedToolbar* advancedToolbar = nullptr;

    activePlayerToolbar->hide();
    advancedToolbar = new AdvancedToolbar(nullptr, activePlayerToolbar); // la toolbar avancée aura les mêmes états que la simple toolbar du player
    if(activePlayer->mediaWidget()->media()) advancedToolbar->enableButtons();

    connect(advancedToolbar, &AdvancedToolbar::playRequest, activePlayer, &PlayerWidget::playFromAdvanced);
    connect(advancedToolbar, &AdvancedToolbar::pauseRequest, activePlayer, &PlayerWidget::pause);
    connect(advancedToolbar, &AdvancedToolbar::stopRequest, activePlayer, &PlayerWidget::stop);
    connect(advancedToolbar, &AdvancedToolbar::ejectRequest, activePlayer, &PlayerWidget::eject);
    connect(advancedToolbar, &AdvancedToolbar::enableMuteRequest, activePlayer, &PlayerWidget::mute);
    connect(advancedToolbar, &AdvancedToolbar::disableMuteRequest, activePlayer, &PlayerWidget::unmute);
    connect(advancedToolbar, &AdvancedToolbar::volumeChanged, activePlayer, &PlayerWidget::setVolume);
    connect(advancedToolbar, &AdvancedToolbar::speedChanged, activePlayer, &PlayerWidget::setSpeed);
    connect(advancedToolbar, &AdvancedToolbar::enableFullscreenRequest, this, &PlayerLayoutManager::enableFullscreenGlobalRequested);
    connect(advancedToolbar, &AdvancedToolbar::disableFullscreenRequest, this, &PlayerLayoutManager::disableFullscreenGlobalRequested);
    connect(advancedToolbar, &AdvancedToolbar::screenshotRequest, activePlayer, &PlayerWidget::takeScreenshot);
    connect(advancedToolbar, &AdvancedToolbar::enableLoopModeRequest, activePlayer, &PlayerWidget::enableLoopMode);
    connect(advancedToolbar, &AdvancedToolbar::disableLoopModeRequest, activePlayer, &PlayerWidget::disableLoopMode);
    connect(advancedToolbar, &AdvancedToolbar::setPositionRequested, activePlayer, &PlayerWidget::setTime);
    connect(advancedToolbar, &AdvancedToolbar::moveTimeBackwardRequested, activePlayer, &PlayerWidget::moveTimeBackward);
    connect(advancedToolbar, &AdvancedToolbar::moveTimeForwardRequested, activePlayer, &PlayerWidget::moveTimeForward);
    connect(advancedToolbar, &AdvancedToolbar::previousMediaRequested, this, &PlayerLayoutManager::previousMediaRequested);
    connect(advancedToolbar, &AdvancedToolbar::nextMediaRequested, this, &PlayerLayoutManager::nextMediaRequested);
    connect(advancedToolbar, &AdvancedToolbar::enableRecordRequested, activePlayer, &PlayerWidget::startRecord);
    connect(advancedToolbar, &AdvancedToolbar::disableRecordRequested, activePlayer, &PlayerWidget::endRecord);
    connect(activePlayer, &PlayerWidget::mediaPlayerLoaded, advancedToolbar, &AdvancedToolbar::enableButtons);
    connect(activePlayer, &PlayerWidget::mediaPlayerEjected, advancedToolbar, &AdvancedToolbar::disableButtons);

    connect(advancedToolbar, &SimpleToolbar::duplicatePlayerRequested, this, [this, activePlayer]() {
        this->duplicatePlayer(activePlayer);
    });

    connect(activePlayer, &PlayerWidget::updateSliderRangeRequest, advancedToolbar, &AdvancedToolbar::updateSliderRange);
    connect(activePlayer, &PlayerWidget::vlcTimeChanged, advancedToolbar, &AdvancedToolbar::updateSliderValue);
    connect(activePlayer, &PlayerWidget::updateFpsRequested, advancedToolbar, &SimpleToolbar::updateFps);

    connect(activePlayer, &PlayerWidget::playUiUpdateRequested, advancedToolbar, &SimpleToolbar::playUiUpdate);
    connect(activePlayer, &PlayerWidget::pauseUiUpdateRequested, advancedToolbar, &SimpleToolbar::pauseUiUpdate);
    connect(activePlayer, &PlayerWidget::muteUiUpdateRequested, advancedToolbar, &SimpleToolbar::muteUiUpdate);
    connect(activePlayer, &PlayerWidget::unmuteUiUpdateRequested, advancedToolbar, &SimpleToolbar::unmuteUiUpdate);
    connect(activePlayer, &PlayerWidget::ejectUiUpdateRequested, advancedToolbar, &SimpleToolbar::ejectUiUpdate);
    connect(activePlayer, &PlayerWidget::stopUiUpdateRequested, advancedToolbar, &SimpleToolbar::stopUiUpdate);
    connect(&SignalManager::instance(), &SignalManager::mediaVolumeChanged, advancedToolbar, &AdvancedToolbar::volumeUiUpdate);
    connect(&SignalManager::instance(), &SignalManager::mediaSpeedChanged, advancedToolbar, &AdvancedToolbar::speedUiUpdate);
    connect(activePlayer, &PlayerWidget::enableLoopUiUpdateRequested, advancedToolbar, &SimpleToolbar::enableLoopUiUpdate);
    connect(activePlayer, &PlayerWidget::disableLoopUiUpdateRequested, advancedToolbar, &SimpleToolbar::disableLoopUiUpdate);
    connect(activePlayer, &PlayerWidget::nameUiUpdateRequest, advancedToolbar, &SimpleToolbar::nameUiUpdate);

    connect(&SignalManager::instance(), &SignalManager::timelineSetPosition, advancedToolbar, &SimpleToolbar::updateSliderValue);

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
void PlayerLayoutManager::duplicatePlayer(PlayerWidget* toBeDuplicated)
{
    if ( ! toBeDuplicated->mediaWidget()->media() ) return;
    int activePlayerCount = m_activePlayers.size();
    
    if(activePlayerCount < 4) {
        toBeDuplicated->pause(); // le dupliqué mis en pause

        createLayout(activePlayerCount + 1);
        auto* player = m_activePlayers.last();

        bool wasMuted = toBeDuplicated->muted();
        int64_t currentTime = libvlc_media_player_get_time(toBeDuplicated->mediaWidget()->m_player);
        
        bool wasLooping = false;
        if (toBeDuplicated->toolbar() && toBeDuplicated->toolbar()->loopBtn()) {
            wasLooping = toBeDuplicated->toolbar()->loopBtn()->isChecked();
        }

        connect(player->mediaWidget(), &MediaWidget::mediaPlayerLoaded, player, [=]() {// quand le media player est chargé pret a être utilisé :
            
            wasMuted ? player->mute() : player->unmute();
            wasLooping ? player->enableLoopMode() : player->disableLoopMode();

            // on lance puis set time pour trouver la frame
            player->play();
            player->setTime(currentTime);

            // dès que le slider bouge (le chargement est de setTime est fini car vlc a detecté timeChanged), on met en pause 
            connect(player, &PlayerWidget::vlcTimeChanged, player, [player](int64_t) {
                player->pause(); 
            }, Qt::SingleShotConnection); 

        }, Qt::SingleShotConnection); 

        
        player->setMediaFromPath(toBeDuplicated->mediaWidget()->media()->filePath());
    }
}


void PlayerLayoutManager::removePlayer(PlayerWidget* playerToRemove){
    int activePlayerCount = m_activePlayers.size();
    if (activePlayerCount > 1){
        playerToRemove->eject();
        m_activePlayers.removeOne(playerToRemove);
        createLayout(m_activePlayers.size());
    }
}

void PlayerLayoutManager::enablePlayerLayoutFullscreen(PlayerWidget* playerToFullscreen){
    for(auto &IPlayer : m_players){
        if(playerToFullscreen != IPlayer)
            IPlayer->hide();
    }
    emit enableFullscreenPlayerRequested();
}

void PlayerLayoutManager::disablePlayerLayoutFullscreen(PlayerWidget* playerToFullscreen){
    for(auto &IPlayer : m_players){
        IPlayer->show();
    }
    emit disableFullscreenPlayerRequested();
}

void PlayerLayoutManager::enableGlobalLayoutFullscreen(){
    for(auto &IPlayer : m_players){
        IPlayer->show();
        // TODO : set toolbar fullscreen ui
    }
    emit enableFullscreenGlobalRequested();
}

void PlayerLayoutManager::disableGlobalLayoutFullscreen(){
    for(auto &IPlayer : m_players){
        IPlayer->show();
        // TODO : set toolbar default ui
    }
    emit disableFullscreenGlobalRequested();
}



/// @brief Vérifie combien de players sont paused
/// puis retourne un bool correspondant au nouvel état du bouton play/pause de la toolbar globale
bool PlayerLayoutManager::newGlobalPlayState(){

    int activePlayerCount = static_cast<int>(m_activePlayers.size());

    int paused = 0;
    for (int Iplayer = 0; Iplayer < activePlayerCount; Iplayer++) {
        if ( ! m_activePlayers[Iplayer]->playing() ) ++paused; 
    }

    if (paused == activePlayerCount ) {
        return false;
    }else {
        return true;
    }
    
}


/// @brief Envoie le nouvel état du bouton play à la toolbar globale
void PlayerLayoutManager::checkPlayersPlayStatus(){
    if( m_activePlayers.size() == 1 ) return;
    emit setGlobalPlayStateRequested(newGlobalPlayState());
}


/// @brief Vérifie combien de players sont mute
/// puis retourne un bool correspondant au nouvel état du bouton mute de la toolbar globale
bool PlayerLayoutManager::newGlobalMuteState(){
    int activePlayerCount = static_cast<int>(m_activePlayers.size());

    for (int Iplayer = 0; Iplayer < activePlayerCount; Iplayer++) {
        if ( ! m_activePlayers[Iplayer]->muted() ) return false; 
    }

    return true;
    
}

/// @brief Envoie le nouvel état du bouton mute à la toolbar globale
void PlayerLayoutManager::checkPlayersMuteStatus(){
    if( m_activePlayers.size() == 1 ) return;

    emit setGlobalMuteStateRequested(newGlobalMuteState());
}

void PlayerLayoutManager::disableGlobalToolbarButtons()
{
    bool allEmpty = true;
    for(unsigned int IPlayer = 0; IPlayer < m_activePlayers.size(); ++IPlayer){
        if(m_activePlayers[IPlayer]->mediaWidget()->media()){
            return;
        }
    }
    emit buttonsDisabled();
}

void PlayerLayoutManager::arrangePlayerLayout(const PlayerLayoutArrangement& arrangement)
{
    switch(arrangement){
    case PlayerLayoutArrangement::Arrangement1:
        createLayout(1);
        break;
    case PlayerLayoutArrangement::Arrangement2H:
        createLayout(2, arrangement);
        break;
    case PlayerLayoutArrangement::Arrangement2V:
        createLayout(2, arrangement);
        break;
    case PlayerLayoutArrangement::Arrangement3H:
    case PlayerLayoutArrangement::Arrangement3V:
    case PlayerLayoutArrangement::Arrangement3Top:
    case PlayerLayoutArrangement::Arrangement3Bot:
    case PlayerLayoutArrangement::Arrangement3Left:
    case PlayerLayoutArrangement::Arrangement3Right:
        createLayout(3, arrangement);
        break;
    case PlayerLayoutArrangement::Arrangement4:
        createLayout(4);
        break;
    case PlayerLayoutArrangement::ArrangementUnknown:
        break;

    }
}


QStringList PlayerLayoutManager::getActivePlayersMediaPath(){
    QStringList mediaPaths;
    for(auto& IActivePlayer : m_activePlayers){
        mediaPaths.append(IActivePlayer->getMediaPath());
    }
    return mediaPaths;
}