#include "PlayerLayoutManager.h"
#include "PlayerWidget.h"
#include "Toolbars/Toolbar.h"
#include "Toolbars/GlobalToolbar.h"
#include "Toolbars/AdvancedToolbar.h"
#include "Project/ProjectManager.h"
#include "GenericDialog.h"

#include <QObject>
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QtAssert>

// #include <iostream>

namespace {

// Valeur utilisée tant que le splitter n'a pas encore de géométrie réelle.
constexpr int c_splitterFallbackSize = 100000;

// Répartit également les panneaux d'un splitter, en pixels réels si possible.
void equalizeSplitter(QSplitter* splitter)
{
    const int splitterCount = splitter->count(); // nombre de widgets dans le splitter
    if (splitterCount <= 0) return;

    const int size = (splitter->orientation() == Qt::Horizontal) ? splitter->width() : splitter->height();
    const int splitterHandlesSize = splitter->handleWidth() * (splitterCount - 1);

    int singleWidgetSize = (size > splitterHandlesSize) ? (size - splitterHandlesSize) / splitterCount : 0;
    if (singleWidgetSize <= 0) singleWidgetSize = c_splitterFallbackSize; // pas encore de géométrie

    QList<int> sizes;
    sizes.reserve(splitterCount);
    for (int IWidget = 0; IWidget < splitterCount; ++IWidget) {
        splitter->setStretchFactor(IWidget, 1); 
        sizes.append(singleWidgetSize);
    }
    splitter->setSizes(sizes);
}

// égalise tous les splitters (imbriqués compris) d'un widget.
void equalizeAllSplitters(QWidget* root)
{
    if (!root) return;
    if (auto* s = qobject_cast<QSplitter*>(root)) equalizeSplitter(s);
    const auto splitters = root->findChildren<QSplitter*>();
    for (QSplitter* s : splitters) equalizeSplitter(s);
}

} // namespace

PlayerLayoutManager::PlayerLayoutManager(QObject *parent)
    : QWidget{}
{

    m_players.reserve(s_maxPlayerCount);
    connect(&SignalManager::instance(), &SignalManager::newArrangementRequested, this, &PlayerLayoutManager::arrangePlayerLayout);
    connect(&SignalManager::instance(), &SignalManager::playerWidgetMediaDropped, this, &PlayerLayoutManager::createLayoutFromPaths);
    connect(&SignalManager::instance(), &SignalManager::playlistEjectPlayer, this, &PlayerLayoutManager::handlePlaylistEject);

    for (size_t IPlayer = 0; IPlayer < s_maxPlayerCount; IPlayer++){
        PlayerWidget* player = new PlayerWidget(this);
        connect(player, &PlayerWidget::duplicatePlayerRequest, this, &PlayerLayoutManager::duplicatePlayer);
        connect(player, &PlayerWidget::removePlayerRequest, this, &PlayerLayoutManager::removePlayer);
        connect(player, &PlayerWidget::enablePlayerFullscreenRequested, this, &PlayerLayoutManager::enablePlayerLayoutFullscreen);
        connect(player, &PlayerWidget::disablePlayerFullscreenRequested, this, &PlayerLayoutManager::disablePlayerLayoutFullscreen);
        connect(player, &PlayerWidget::checkPlayersPlayStatusRequested, this, &PlayerLayoutManager::checkPlayersPlayStatus);
        connect(player, &PlayerWidget::checkPlayersMuteStatusRequested, this, &PlayerLayoutManager::checkPlayersMuteStatus);
        connect(player, &PlayerWidget::enableZoomUiUpdateRequested, this, &PlayerLayoutManager::checkPlayersZoomStatus);
        connect(player, &PlayerWidget::disableZoomUiUpdateRequested, this, &PlayerLayoutManager::checkPlayersZoomStatus);
        m_players.append(player);
    }
    
}

PlayerLayoutManager::~PlayerLayoutManager()
{
    m_players.clear();
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

    if(count != 1){
        m_activePlayers[0]->resetLayerWidgets();
    }

    switch (count){
        case 1: 
            container = create1();
            player = m_activePlayers[0];
            emit enableNavPanelRequested();
            break;
        case 2:
            {
                Qt::Orientation orientation = arrangement == Arrangement2V ? Qt::Vertical : Qt::Horizontal;
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
    updateActivePlayersMediaState();
    emit updateContainerRequest(player, container, toolbar);

    // une fois le container affiché (donc dimensionné), répartit les panneaux à parts égales
    if (container) QTimer::singleShot(0, container, [container]{ equalizeAllSplitters(container); });
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
    updateActivePlayersMediaState();
    emit updateContainerRequest(player, container, toolbar);

    if (container) QTimer::singleShot(0, container, [container]{ equalizeAllSplitters(container); });
}

/// @brief Fonction appelé par le project manager quand on charge un projet.
/// ProjectManager::instance().requestProjectCreation(getActivePlayersMediaPath()); aurait reset le projet créer 
/// @param filesPaths 
void PlayerLayoutManager::createLayoutFromProject(const QStringList& filesPaths){
    QWidget* container = create1(filesPaths);
    PlayerWidget* player = m_activePlayers[0];
    emit enableNavPanelRequested();

    auto* toolbar = createLayoutToolbar();
    updateActivePlayersMediaState();
    emit updateContainerRequest(player, container, toolbar);

    if (container) QTimer::singleShot(0, container, [container]{ equalizeAllSplitters(container); });
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
    m_currentArrangement = Arrangement1;

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
        splitter->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));

    }

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(splitter);
    m_currentArrangement = orientation == Qt::Horizontal ? Arrangement2H : Arrangement2V;

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
    m_currentArrangement = arrangement;

    if(arrangement == Arrangement3V || arrangement == Arrangement3H ){

        mainSplitter->setOrientation(arrangement == Arrangement3V ? Qt::Vertical : Qt::Horizontal);

        mainSplitter->addWidget(m_activePlayers[0]);
        mainSplitter->addWidget(m_activePlayers[1]);
        mainSplitter->addWidget(m_activePlayers[2]);

        mainSplitter->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize, c_splitterFallbackSize})); // mettre INT_MAX, ne fonctionne pas

        mainSplitter->setStretchFactor(0, 1);
        mainSplitter->setStretchFactor(1, 1);
        mainSplitter->setStretchFactor(2, 1);

    } else {
        
        if(arrangement == Arrangement3Bot || arrangement == Arrangement3Top){

            mainSplitter->setOrientation(Qt::Vertical);

            auto *adjacentPlayers = new QSplitter(Qt::Horizontal);
            if(arrangement == Arrangement3Bot){
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

            adjacentPlayers->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));

        }
        else{

            mainSplitter->setOrientation(Qt::Horizontal);

            auto *adjacentPlayers = new QSplitter(Qt::Vertical);

            if(arrangement == Arrangement3Right){
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
                // m_currentArrangement = Arrangement3Left;
            }

            adjacentPlayers->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));
        }
        mainSplitter->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));
    }

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
    top->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));

    bottom->addWidget(m_activePlayers[2]);
    bottom->addWidget(m_activePlayers[3]);
    bottom->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));

    mainSplitter->addWidget(top);
    mainSplitter->addWidget(bottom);
    mainSplitter->setSizes(QList<int>({c_splitterFallbackSize, c_splitterFallbackSize}));

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainSplitter);
    m_currentArrangement = Arrangement4;

    return container;
}

Toolbar* PlayerLayoutManager::createGlobalToolbar(){
    GlobalToolbar* globalToolbar = new GlobalToolbar(nullptr);

    // Parcours les players pour connecter le play / play de la global a ses players
    for(auto& IActivePlayer : m_activePlayers){
        IActivePlayer->toolbar()->show();
        IActivePlayer->toolbar()->setReplacedByAdvanced(false);

        connect(globalToolbar, &GlobalToolbar::playRequest, IActivePlayer, &PlayerWidget::play);
        connect(globalToolbar, &GlobalToolbar::pauseRequest, IActivePlayer, &PlayerWidget::pause);
        connect(globalToolbar, &GlobalToolbar::stopRequest, IActivePlayer, &PlayerWidget::stop);
        connect(globalToolbar, &GlobalToolbar::ejectRequest, IActivePlayer, &PlayerWidget::eject);
        connect(globalToolbar, &GlobalToolbar::enableMute, IActivePlayer, &PlayerWidget::mute);
        connect(globalToolbar, &GlobalToolbar::disableMute, IActivePlayer, &PlayerWidget::unmute);
        connect(globalToolbar, &GlobalToolbar::enableZoomMode, IActivePlayer, &PlayerWidget::enableZoomMode);
        connect(globalToolbar, &GlobalToolbar::disableZoomMode, IActivePlayer, &PlayerWidget::disableZoomMode);
        // connect(globalToolbar, &GlobalToolbar::screenshotRequest, IActivePlayer, &PlayerWidget::takeScreenshot);
        connect(IActivePlayer, &PlayerWidget::mediaPlayerLoaded, globalToolbar, &GlobalToolbar::enableButtons);
        connect(IActivePlayer, &PlayerWidget::mediaPlayerEjected, this, &PlayerLayoutManager::disableGlobalToolbarButtons);
    }

    connect(globalToolbar, &Toolbar::enableFullscreenRequest, this, &PlayerLayoutManager::enableGlobalLayoutFullscreen);
    connect(globalToolbar, &Toolbar::enableFullscreenRequest, globalToolbar, &GlobalToolbar::enableFullscreenUiUpdate);
    connect(globalToolbar, &Toolbar::disableFullscreenRequest, this, &PlayerLayoutManager::disableGlobalLayoutFullscreen);
    connect(globalToolbar, &Toolbar::disableFullscreenRequest, globalToolbar, &GlobalToolbar::disableFullscreenUiUpdate);
    connect(this, &PlayerLayoutManager::buttonsDisabled, globalToolbar, &GlobalToolbar::disableButtons);
    connect(globalToolbar, &Toolbar::screenshotRequest, this, &PlayerLayoutManager::takeGlobalScreenshot);
    
    globalToolbar->muteBtn()->setButtonState(newGlobalMuteState());
    globalToolbar->playPauseBtn()->setButtonState(newGlobalPlayState());

    disableGlobalToolbarButtons();

    return static_cast<Toolbar*>(globalToolbar);
}

Toolbar* PlayerLayoutManager::createAdvancedToolbar(){
    Q_ASSERT(m_activePlayers.size() == 1);

    auto* activePlayer = m_activePlayers[0];
    auto* activePlayerToolbar = activePlayer->toolbar();

    AdvancedToolbar* advancedToolbar = nullptr;

    //activePlayerToolbar->hide();
    activePlayerToolbar->setReplacedByAdvanced(true);
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

    // garde les slider de la toolbar simple du player synchronisé
    auto syncSlider = [](QSlider* slider, int value){
        slider->blockSignals(true);
        slider->setValue(value);
        slider->blockSignals(false);
    };
    connect(advancedToolbar, &AdvancedToolbar::volumeChanged, activePlayerToolbar, [activePlayerToolbar, syncSlider](int v){ 
        syncSlider(activePlayerToolbar->volumeSlider(), v); 
    });
    connect(advancedToolbar, &AdvancedToolbar::speedChanged, activePlayerToolbar, [activePlayerToolbar, syncSlider](int v){ 
        syncSlider(activePlayerToolbar->speedSlider(), v); 
    });

    connect(advancedToolbar, &AdvancedToolbar::enableFullscreenRequest, this, &PlayerLayoutManager::enableFullscreenGlobalRequested);
    connect(advancedToolbar, &AdvancedToolbar::enableFullscreenRequest, advancedToolbar, &SimpleToolbar::enableFullscreenUiUpdate);
    connect(advancedToolbar, &AdvancedToolbar::disableFullscreenRequest, this, &PlayerLayoutManager::disableFullscreenGlobalRequested);
    connect(advancedToolbar, &AdvancedToolbar::disableFullscreenRequest, advancedToolbar, &SimpleToolbar::disableFullscreenUiUpdate);
    connect(advancedToolbar, &AdvancedToolbar::screenshotRequest, activePlayer, &PlayerWidget::takeScreenshot);
    connect(advancedToolbar, &AdvancedToolbar::enableLoopModeRequest, activePlayer, &PlayerWidget::enableLoopMode);
    connect(advancedToolbar, &AdvancedToolbar::disableLoopModeRequest, activePlayer, &PlayerWidget::disableLoopMode);
    connect(advancedToolbar, &AdvancedToolbar::setPositionRequested, activePlayer, &PlayerWidget::setTime);
    connect(advancedToolbar, &AdvancedToolbar::moveTimeBackwardRequested, activePlayer, &PlayerWidget::moveTimeBackward);
    connect(advancedToolbar, &AdvancedToolbar::moveTimeForwardRequested, activePlayer, &PlayerWidget::moveTimeForward);
    connect(advancedToolbar, &AdvancedToolbar::rotateRequested, activePlayer, &PlayerWidget::rotate);
    connect(advancedToolbar, &AdvancedToolbar::hFlipRequested, activePlayer->mediaWidget(), &MediaWidget::hFlip);
    connect(advancedToolbar, &AdvancedToolbar::vFlipRequested, activePlayer->mediaWidget(), &MediaWidget::vFlip);
    connect(advancedToolbar, &AdvancedToolbar::prevFrameRequested, activePlayer->mediaWidget(), &MediaWidget::prevFrame);
    connect(advancedToolbar, &AdvancedToolbar::nextFrameRequested, activePlayer->mediaWidget(), &MediaWidget::nextFrame);
    connect(advancedToolbar, &AdvancedToolbar::previousMediaRequested, this, &PlayerLayoutManager::previousMediaRequested);
    connect(advancedToolbar, &AdvancedToolbar::nextMediaRequested, this, &PlayerLayoutManager::nextMediaRequested);
    connect(advancedToolbar, &AdvancedToolbar::enableRecordRequested, activePlayer, &PlayerWidget::startRecord);
    connect(advancedToolbar, &AdvancedToolbar::disableRecordRequested, activePlayer, &PlayerWidget::endRecord);
    connect(advancedToolbar, &AdvancedToolbar::extractSequenceRequest, activePlayer, &PlayerWidget::openSequenceExtractionDialog);
    connect(advancedToolbar, &AdvancedToolbar::mediaInformationRequest, activePlayer->mediaWidget(), &MediaWidget::openMediaInfoDialog);
    connect(advancedToolbar, &AdvancedToolbar::enableZoomMode, activePlayer->mediaWidget(), &MediaWidget::enableZoomMode);
    connect(advancedToolbar, &AdvancedToolbar::disableZoomMode, activePlayer->mediaWidget(), &MediaWidget::disableZoomMode);
    connect(advancedToolbar, &AdvancedToolbar::subtitlesFileDialogRequested, activePlayer, &PlayerWidget::openSubtitlesFileDialog);
    connect(advancedToolbar->getExtendedToolbar(), &ExtensionToolbar::adjustmentChangeRequested, activePlayer->mediaWidget(), &MediaWidget::adjustMedia);
    connect(advancedToolbar->getExtendedToolbar(), &ExtensionToolbar::resetAdjustmentsRequested, activePlayer->mediaWidget(), &MediaWidget::resetAdjustments);
    connect(activePlayer, &PlayerWidget::mediaPlayerLoaded, advancedToolbar, &AdvancedToolbar::enableButtons);
    connect(activePlayer, &PlayerWidget::mediaPlayerLoaded, this, &PlayerLayoutManager::updateActivePlayersMediaState);
    connect(activePlayer, &PlayerWidget::mediaPlayerEjected, advancedToolbar, &AdvancedToolbar::disableButtons);
    connect(activePlayer, &PlayerWidget::mediaPlayerEjected, this, &PlayerLayoutManager::updateActivePlayersMediaState);

    // Redirection audio/sous-titres vers la toolbar avancée (toolbar simple cachée)
    //disconnect(activePlayer->mediaWidget(), &MediaWidget::updateAudioTracksRequested, activePlayerToolbar, &SimpleToolbar::updateAudioTracks);
    //disconnect(activePlayer->mediaWidget(), &MediaWidget::updateSubtitlesTracksRequested, activePlayerToolbar, &SimpleToolbar::updateSubtitlesTracks);
    connect(activePlayer->mediaWidget(), &MediaWidget::updateAudioTracksRequested, advancedToolbar, &SimpleToolbar::updateAudioTracks);
    connect(activePlayer->mediaWidget(), &MediaWidget::updateSubtitlesTracksRequested, advancedToolbar, &SimpleToolbar::updateSubtitlesTracks);

    connect(activePlayer->mediaWidget(), &MediaWidget::setAudioTrackRequested, advancedToolbar, &SimpleToolbar::setAudioTrackDefault);
    connect(activePlayer->mediaWidget(), &MediaWidget::setSubtitlesTrackRequested, advancedToolbar, &SimpleToolbar::setSubtitlesTrackDefault);

    connect(activePlayer->mediaWidget(), &MediaWidget::subtitleTrackAdded, advancedToolbar, &SimpleToolbar::subtitleTrackAdd);

    // Connecte choix audio/sous-titres au mediawidget
    connect(advancedToolbar, &SimpleToolbar::setAudioTrackRequested, activePlayer->mediaWidget(), &MediaWidget::setAudioTrack);
    connect(advancedToolbar, &SimpleToolbar::setSubtitlesTrackRequested, activePlayer->mediaWidget(), &MediaWidget::setSubtitleTrack);

    // garde la piste de sous-titres de la toolbar simple du player synchronisée avec l'advanced
    connect(advancedToolbar, &SimpleToolbar::setSubtitlesTrackRequested, activePlayerToolbar, &SimpleToolbar::setSubtitlesTrackDefault);

    // garde le mode "temps restant" (durationBtn) de la toolbar simple synchronisé avec l'advanced
    connect(advancedToolbar->durationBtn(), &QPushButton::toggled, activePlayerToolbar->durationBtn(), &QPushButton::setChecked);

    // Synchro piste actuelle à l'advanced toolbar si le média est déjà chargé
    if (activePlayer->mediaWidget()->media()) {
        advancedToolbar->updateAudioTracks(activePlayer->mediaWidget()->audioTracks());
        advancedToolbar->updateSubtitlesTracks(activePlayer->mediaWidget()->subtitlesTracks());
        advancedToolbar->setAudioTrackDefault(activePlayerToolbar->currentAudioTrackIndex());
        advancedToolbar->setSubtitlesTrackDefault(activePlayerToolbar->currentSubtitlesTrackId());
    }

    connect(advancedToolbar, &AdvancedToolbar::showBlackOpacityModeRequested, activePlayer, &PlayerWidget::setBlackOpacityMode);

    connect(advancedToolbar, &AdvancedToolbar::setOverlayModeRequested, activePlayer, &PlayerWidget::setOverlayMode);
    
    connect(advancedToolbar, &AdvancedToolbar::showDrawingModeRequested, activePlayer, &PlayerWidget::showDrawingMode);
    connect(advancedToolbar, &AdvancedToolbar::showDrawingModeRequested, activePlayer, &PlayerWidget::pause);

    connect(advancedToolbar, &SimpleToolbar::duplicatePlayerRequested, this, [this, activePlayer]() {
        activePlayer->resetLayerWidgets();
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
    connect(activePlayer->mediaWidget(), &MediaWidget::hFlipUiUpdateRequested, advancedToolbar, &AdvancedToolbar::hFlipUiUpdate);
    connect(activePlayer->mediaWidget(), &MediaWidget::vFlipUiUpdateRequested, advancedToolbar, &AdvancedToolbar::vFlipUiUpdate);
    connect(activePlayer->mediaWidget(), &MediaWidget::zoomValueUpdated, advancedToolbar, &AdvancedToolbar::setZoomIndicatorText);
    connect(activePlayer->mediaWidget(), &MediaWidget::rotationTooltipUpdateRequested, advancedToolbar, &AdvancedToolbar::updateRotationTooltip);
    connect(activePlayer->mediaWidget(), &MediaWidget::flipTooltipUpdateRequested, advancedToolbar, &AdvancedToolbar::updateFlipTooltip);

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

        int oldVolume = toBeDuplicated->toolbar()->volumeSlider()->value();
        int oldSpeed = toBeDuplicated->toolbar()->speedSlider()->value();
        int oldAudioTrackIndex = toBeDuplicated->toolbar()->currentAudioTrackIndex();
        int oldSubtitlesTrackId = toBeDuplicated->toolbar()->currentSubtitlesTrackId();
        bool oldShowRemainingTime = toBeDuplicated->toolbar()->durationBtn()->isChecked();

        connect(player->mediaWidget(), &MediaWidget::mediaPlayerLoaded, player, [=]() {// quand le media player est chargé pret a être utilisé :

            wasMuted ? player->mute() : player->unmute();
            wasLooping ? player->enableLoopMode() : player->disableLoopMode();

            player->toolbar()->volumeSlider()->setValue(oldVolume);
            player->toolbar()->speedSlider()->setValue(oldSpeed);
            player->toolbar()->setAudioTrackDefault(oldAudioTrackIndex);
            player->toolbar()->setSubtitlesTrackDefault(oldSubtitlesTrackId);
            player->toolbar()->durationBtn()->setChecked(oldShowRemainingTime);

            // on lance la lecture et le seek
            player->play();
            player->setTime(currentTime);

            // quand timechanged recu par le nouveau player, on pause (on est au temps demandé)
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(player, &PlayerWidget::vlcTimeChanged, player, [player, currentTime, conn](int64_t ) {
                    QObject::disconnect(*conn);
                    player->pause();
                    emit player->vlcTimeChanged(currentTime); // sur mac le timeedit pas mis à jour, force la mise à jour ici
                });

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
    for(auto &IPlayer : m_activePlayers){
        if(playerToFullscreen != IPlayer) {
            IPlayer->toolbar()->setDefaultUI();
            IPlayer->hide();
        }
        else IPlayer->toolbar()->setFullscreenUI(false);
    }
    emit enableFullscreenPlayerRequested();
}

void PlayerLayoutManager::disablePlayerLayoutFullscreen(PlayerWidget* playerToFullscreen){
    for(auto &IPlayer : m_activePlayers){
        IPlayer->show();
        IPlayer->toolbar()->setDefaultUI();
    }
    emit disableFullscreenPlayerRequested();
}

void PlayerLayoutManager::enableGlobalLayoutFullscreen(){
    for(auto &IPlayer : m_activePlayers){
        IPlayer->show();
        IPlayer->toolbar()->setFullscreenUI(true);
    }
    emit enableFullscreenGlobalRequested();
}

void PlayerLayoutManager::disableGlobalLayoutFullscreen(){
    for(auto &IPlayer : m_activePlayers){
        IPlayer->show();
        IPlayer->toolbar()->setDefaultUI();
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

/// @brief Vérifie combien de players sont zoomés
/// puis retourne un bool correspondant au nouvel état du bouton zoom de la toolbar globale
bool PlayerLayoutManager::newGlobalZoomState()
{
    int activePlayerCount = static_cast<int>(m_activePlayers.size());

    for (int Iplayer = 0; Iplayer < activePlayerCount; Iplayer++) {
        if ( !m_activePlayers[Iplayer]->zoomed() ) return false;
    }

    return true;
}

/// @brief Envoie le nouvel état du bouton zoom à la toolbar globale
void PlayerLayoutManager::checkPlayersZoomStatus(){
    if( m_activePlayers.size() == 1 ) return;

    emit setGlobalZoomStateRequested(newGlobalZoomState());
}

void PlayerLayoutManager::disableGlobalToolbarButtons()
{
    for(unsigned int IPlayer = 0; IPlayer < m_activePlayers.size(); ++IPlayer){
        if(m_activePlayers[IPlayer]->mediaWidget()->media()){
            return;
        }
    }
    emit buttonsDisabled(); 
    updateActivePlayersMediaState();
}

void PlayerLayoutManager::updateActivePlayersMediaState()
{
    bool isSingle = (m_activePlayers.size() == 1);
    emit activePlayersCountChanged(isSingle);

    bool enabled = false;

    for (PlayerWidget* player : m_activePlayers) {
        if (player->mediaWidget() && player->mediaWidget()->media()) {
            enabled = true; 
            break;     
        }
    }
    
    emit activePlayersMediaStateChanged(enabled);
}

void PlayerLayoutManager::arrangePlayerLayout(const PlayerLayoutArrangement& arrangement)
{
    m_currentArrangement = arrangement;
    switch(arrangement){
    case Arrangement1:
        createLayout(1);
        break;
    case Arrangement2H:
        createLayout(2, arrangement);
        break;
    case Arrangement2V:
        createLayout(2, arrangement);
        break;
    case Arrangement3H:
    case Arrangement3V:
    case Arrangement3Top:
    case Arrangement3Bot:
    case Arrangement3Left:
    case Arrangement3Right:
        createLayout(3, arrangement);
        break;
    case Arrangement4:
        createLayout(4);
        break;
    case ArrangementUnknown:
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

QList<int> PlayerLayoutManager::getActivePlayersCurrentTimes()
{
    QList<int> currentTimes;
    for(auto& IActivePlayer : m_activePlayers){
        currentTimes.append(IActivePlayer->getCurrentTime());
    }
    return currentTimes;
}

QList<GlobalScreenshotPlayerData> PlayerLayoutManager::getActivePlayersData(){
    QList<GlobalScreenshotPlayerData> activePlayersData;
    for(auto& IActivePlayer : m_activePlayers){
        activePlayersData.append({IActivePlayer->getMediaPath(), IActivePlayer->getCurrentTime(), IActivePlayer->getSar()});
    }
    return activePlayersData;
}

void PlayerLayoutManager::takeGlobalScreenshot()
{
    GlobalScreenshotHelper* globalScreenshot = new GlobalScreenshotHelper(getActivePlayersData(), m_currentArrangement);

    QObject::connect(globalScreenshot, &QThread::finished, globalScreenshot, &QObject::deleteLater);
    QObject::connect(globalScreenshot, &GlobalScreenshotHelper::finishedSuccess, this, [this](){
        QMessageBox::information(this, "", PrefManager::instance().getText("messagebox_global_screenshot_completed"));
    });
    QObject::connect(globalScreenshot, &GlobalScreenshotHelper::finishedError, this, [this](){
        QMessageBox::critical(this, "", PrefManager::instance().getText("messagebox_global_screenshot_error"));
    });

    globalScreenshot->start();
}

void PlayerLayoutManager::showAllActivePlayersToolbars(bool visible) {
    for (PlayerWidget* player : m_activePlayers) {
        if (player && player->toolbar()) {
            visible ? player->toolbar()->showAnimation() : player->toolbar()->hideAnimation();
        }
    }
}

void PlayerLayoutManager::handlePlaylistEject()
{
    if(m_activePlayers.size() <= 0) return;
    PlayerWidget* player = m_activePlayers[0];
    if(player) player->eject();
}