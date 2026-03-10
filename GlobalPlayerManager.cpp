#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include "ProjectManager.h"

#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    layout = new QVBoxLayout();
    mainLayout->addLayout(layout);
    m_navPanel = new NavPanel(this);
    mainLayout->addWidget(m_navPanel);

    m_layoutManager = new PlayerLayoutManager();
    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenPlayerRequested, this, &GlobalPlayerManager::enableFullscreenPlayer);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenPlayerRequested, this, &GlobalPlayerManager::disableFullscreenPlayer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, &GlobalPlayerManager::enableFullscreenMainRequested);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, &GlobalPlayerManager::disableFullscreenMainRequested);

    connect(m_layoutManager, &PlayerLayoutManager::setGlobalPlayStateRequested, this, &GlobalPlayerManager::setGlobalPlayState);
    connect(m_layoutManager, &PlayerLayoutManager::setGlobalMuteStateRequested, this, &GlobalPlayerManager::setGlobalMuteState);

    connect(m_layoutManager, &PlayerLayoutManager::disableNavPanelRequested, this, &GlobalPlayerManager::disableNavPanelRequested);
    connect(m_layoutManager, &PlayerLayoutManager::enableNavPanelRequested, this, &GlobalPlayerManager::enableNavPanelRequested);

    connect(m_navPanel, &NavPanel::openMediaFileRequested, m_layoutManager, [this](const QString &filePath)
        { m_layoutManager->createLayoutFromPaths(QStringList(filePath)); qDebug() << "connexion russie " << filePath; }
    );
    connect(&ProjectManager::instance(), &ProjectManager::projectInitialized, this, &GlobalPlayerManager::createTimelineWidget);
    connect(&ProjectManager::instance(), &ProjectManager::projectDeleted, this, &GlobalPlayerManager::disableSegmentation);

    m_layoutManager->createLayout(1);
}

///@brief Supprime puis Remplace m_playersWidget par le widget retourné par m_layoutManager->createLayoutFromPaths(filesPaths); 
void GlobalPlayerManager::setPlayersFromPaths(QStringList filesPaths)
{
    m_layoutManager->createLayoutFromPaths(filesPaths);
}

/// @brief Met à jour le widget qui contient les playerWidgets et la toolbar avancée ou globale.
/// @param videoPlayersCount Nombre de PlayerWidgets dans le container
/// @param newPlayersWidget Le widget à ajouter au layout
/// @param newToolbar La GlobalToolbar si videoPlayersCount != 1, AdvancedToolbar sinon
void GlobalPlayerManager::updateContainer(Media* media, QWidget * newPlayersWidget, Toolbar* newToolbar)
{
    // clean ancienne UI
    if (m_toolbarWidget){
        m_toolbarWidget->disconnect();
        layout->removeWidget(m_toolbarWidget);
        m_toolbarWidget->deleteLater(); 
        m_toolbarWidget = nullptr;
    }
    if (m_playersWidget) {
        layout->removeWidget(m_playersWidget);
        m_playersWidget->deleteLater(); 
        m_playersWidget = nullptr;
    }

    // ajout du nouveau playerWidget et toolbar 
    if (newPlayersWidget){
        m_playersWidget = newPlayersWidget;
        layout->addWidget(m_playersWidget);
    }
    if (newToolbar){
        m_toolbarWidget = newToolbar;
        layout->addWidget(m_toolbarWidget);
    }

    if(media){
        ProjectManager::instance().createProject(media);
        auto *advancedToolbar = static_cast<AdvancedToolbar*>(m_toolbarWidget);
        connect(advancedToolbar, &AdvancedToolbar::previousMediaRequested, this, &GlobalPlayerManager::playPreviousMedia);
        connect(advancedToolbar, &AdvancedToolbar::nextMediaRequested, this, &GlobalPlayerManager::playNextMedia);
        connect(m_navPanel, &NavPanel::disableToolbarLoopRequested, advancedToolbar, &AdvancedToolbar::disableLoopMode);
    }
}

/// @brief Ouvre la fenêtre à droite de l'écran contenant des infos sur la playlist ou sur le plan sélectionné en fonction du mode
void GlobalPlayerManager::openNavPanel()
{
    m_navPanel->showPanel();
}

void GlobalPlayerManager::closeNavPanel()
{
    m_navPanel->hidePanel();
}
// slots

/// @brief Met à jour l'état du bouton play pause 
/// @param state 
void GlobalPlayerManager::setGlobalPlayState(bool state)
{
    if(m_toolbarWidget){
        m_toolbarWidget->playPauseBtn()->setButtonState(state);
    }
}

/// @brief Met à jour l'état du bouton mute
/// @param state 
void GlobalPlayerManager::setGlobalMuteState(bool state)
{
    if(m_toolbarWidget){
        m_toolbarWidget->muteBtn()->setButtonState(state);
    }
}

/// @brief Cache la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::enableFullscreenPlayer()
{
    if(m_toolbarWidget)
        m_toolbarWidget->hide();
    emit enableFullscreenMainRequested();
}

/// @brief Affiche la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::disableFullscreenPlayer()
{
    if(m_toolbarWidget)
        m_toolbarWidget->show();
    emit disableFullscreenMainRequested();
}

/// @brief Cache la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::enableSegmentation()
{
    if(m_timeline){
        m_timeline->show();
    }
}

void GlobalPlayerManager::disableSegmentation()
{
    if(m_timeline){
        m_timeline->hide();
    }
}

void GlobalPlayerManager::playPreviousMedia()
{
    m_navPanel->playPreviousMedia();
}

void GlobalPlayerManager::playNextMedia()
{
    m_navPanel->playNextMedia();
}

void GlobalPlayerManager::createTimelineWidget()
{
    auto* toolbar = static_cast<AdvancedToolbar*>(m_toolbarWidget);
    connect(toolbar, &AdvancedToolbar::enableSegmentationRequest, this, &GlobalPlayerManager::enableSegmentation);
    connect(toolbar, &AdvancedToolbar::disableSegmentationRequest, this, &GlobalPlayerManager::disableSegmentation);
    
    if(m_timeline){
        m_timeline->deleteLater();
        m_timeline = nullptr;
    }
    
    m_timeline = new TimelineWidget(this);
    m_timeline->setFixedHeight(150);
    connect(toolbar, &SimpleToolbar::setCursorPositionRequested, m_timeline, &TimelineWidget::updateCursorPos);
    connect(m_timeline, &TimelineWidget::updateShotDetailRequested, &SignalManager::instance(), &SignalManager::timelineWidgetUpdateShotDetail);
    layout->addWidget(m_timeline);
    m_timeline->hide();
}


