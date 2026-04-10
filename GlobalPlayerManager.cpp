#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include "Project/ProjectManager.h"

#include <qlayout.h>

#include <QDebug>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    mainLayout->addLayout(layout);
    //m_navPanel = new NavPanel(this, m_shotInfoController);
    m_navPanel = new NavPanel(this);
    mainLayout->addWidget(m_navPanel);

    m_layoutManager = new PlayerLayoutManager();

    connect(&ProjectManager::instance(), &ProjectManager::loadMediaProjectRequested, m_layoutManager, &PlayerLayoutManager::createLayoutFromProject);

    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenPlayerRequested, this, &GlobalPlayerManager::enableFullscreenPlayer);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenPlayerRequested, this, &GlobalPlayerManager::disableFullscreenPlayer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, [this](){
        if(m_toolbarWidget){
            m_toolbarWidget->setFullscreenUI();
        }
        enableFullscreenMainRequested();
    });

    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, [this](){
        if(m_toolbarWidget){
            m_toolbarWidget->setParent(this);
            m_toolbarWidget->setDefaultUI();
            layout->insertWidget(1, m_toolbarWidget);
        }
        disableFullscreenMainRequested();
    });

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
    qDebug() << "GlobalPlayerManager height : " << this->height();
    //m_navPanel->
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
void GlobalPlayerManager::updateContainer(PlayerWidget* player, QWidget * newPlayersWidget, Toolbar* newToolbar)
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

    if(m_timeline){
        layout->removeWidget(m_timeline);
        m_timeline->deleteLater();
        m_timeline = nullptr;
    }

    // ajout du nouveau playerWidget et toolbar 
    if (newPlayersWidget){
        m_playersWidget = newPlayersWidget;
        layout->addWidget(m_playersWidget);
    }
    if (newToolbar){
        m_toolbarWidget = newToolbar;
        layout->addWidget(m_toolbarWidget);
        m_toolbarWidget->setTBParent(this); // since toolbar was created in playerlayoutmanager, need to update its internal m_parent 
    }

    m_player = player;

    auto *advancedToolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget);

    if(advancedToolbar){
        connect(&ProjectManager::instance(), &ProjectManager::ejectMedia, advancedToolbar, &AdvancedToolbar::ejectRequest);

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

void GlobalPlayerManager::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(m_toolbarWidget){
        m_toolbarWidget->updateFullscreenPosition();
    }
}

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
    Q_ASSERT( m_player );

    auto* toolbar = static_cast<AdvancedToolbar*>(m_toolbarWidget);
    connect(toolbar, &AdvancedToolbar::enableSegmentationRequest, this, &GlobalPlayerManager::enableSegmentation);
    connect(toolbar, &AdvancedToolbar::disableSegmentationRequest, this, &GlobalPlayerManager::disableSegmentation);
    
    if(m_timeline){
        m_timeline->deleteLater();
        m_timeline = nullptr;
    }
    ProjectManager& projManager = ProjectManager::instance();
    Project* proj = projManager.projet();
    Media* projMedia = proj->media;

    if( ! projMedia ){
        qCritical() << "Pas de media dans le projet, impossible de créer une timeline";
        return;
    }

    if(projMedia->fps() == 0){
        projMedia->setFps(1);
        qDebug() << "FPS == 0, utilisation de FPS = 1";
    }

    if(projMedia->duration() == 0){
        qCritical() << "Pas de duréer pour le média, impossible de créer une timeline";
        return;
    }


    m_timeline = new TimelineWidget(projMedia->fps(), projMedia->duration(), projMedia->filePath(), proj->shots, this);
    m_timeline->setFixedHeight(150);

    projManager.setTimeline(m_timeline);

    connect(m_player, &PlayerWidget::vlcTimeChanged, m_timeline, &TimelineWidget::updateCursorPos);

    connect(m_timeline, &TimelineWidget::saveNeeded, &projManager, &ProjectManager::setSaveNeeded);

    connect(m_timeline, &TimelineWidget::timelineSetPosition, m_player, &PlayerWidget::setTime);

    connect(m_timeline, &TimelineWidget::updateShotDetailRequest, m_navPanel, &NavPanel::timelineWidgetUpdateShotDetail);
    connect(m_timeline, &TimelineWidget::disableTimeRelatedUI, m_navPanel, &NavPanel::disableShotControlButtons );
    connect(m_timeline, &TimelineWidget::enableTimeRelatedUI, m_navPanel, &NavPanel::enableShotControlButtons );
    connect(m_navPanel, &NavPanel::goToShotRequest, m_timeline, &TimelineWidget::goToShot);

    connect(m_timeline, &TimelineWidget::timelineSliderPositionRequested, toolbar, &SimpleToolbar::updateSliderValue );
    connect(m_timeline, &TimelineWidget::disableTimeRelatedUI, toolbar, &AdvancedToolbar::disableSlider );
    connect(m_timeline, &TimelineWidget::enableTimeRelatedUI, toolbar, &AdvancedToolbar::enableSlider );

    connect(toolbar, &AdvancedToolbar::toolbarCursorPositionRequested, m_timeline, &TimelineWidget::updateCursorVisually);

    layout->addWidget(m_timeline);
    m_timeline->hide();
}


