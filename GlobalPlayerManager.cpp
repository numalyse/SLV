#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"
#include "Toolbars/GlobalToolbar.h"

#include "Project/ProjectManager.h"

#include <QLayout>

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

    m_thumbnailWorker = new ThumbnailWorker(this);
    m_thumbnailWorker->start();

    m_navPanel = new NavPanel(m_thumbnailWorker, this);
    mainLayout->addWidget(m_navPanel);

    m_separationLine = new QFrame();
    m_separationLine->setFrameShape(QFrame::HLine);
    m_separationLine->setFrameShadow(QFrame::Sunken);
    m_separationLine->setLineWidth(1);
    m_separationLine->setContentsMargins(10,0,10,0);

    m_layoutManager = new PlayerLayoutManager();

    connect(&ProjectManager::instance(), &ProjectManager::loadMediaProjectRequested, m_layoutManager, &PlayerLayoutManager::createLayoutFromProject);

    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenPlayerRequested, this, &GlobalPlayerManager::enableFullscreenPlayer);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenPlayerRequested, this, &GlobalPlayerManager::disableFullscreenPlayer);

    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, [this](){
        if(m_toolbarWidget){
            layout->removeWidget(m_separationLine);
            m_separationLine->hide();
            // cast pour appeler la setFullscreenUI avec la bonne marge
            if (auto *globalToolbar = qobject_cast<GlobalToolbar*>(m_toolbarWidget)) {
                globalToolbar->setFullscreenUI(); 
            }
            else if (auto *advancedToolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget)) {
                advancedToolbar->setFullscreenUI(); 
            }
            else {
                m_toolbarWidget->setFullscreenUI();
            }
        }
        enableFullscreenMainRequested();
    });

    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, [this](){
        if(m_toolbarWidget){
            m_toolbarWidget->setParent(this);
            m_toolbarWidget->setDefaultUI();
            
            layout->removeWidget(m_separationLine);
            layout->removeWidget(m_toolbarWidget);

            layout->insertWidget(1, m_separationLine);
            layout->insertWidget(2, m_toolbarWidget);

            auto *advancedToolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget);
            if(!advancedToolbar){
                m_separationLine->show();
            }
        }
        disableFullscreenMainRequested();
    });

    connect(m_layoutManager, &PlayerLayoutManager::setGlobalPlayStateRequested, this, &GlobalPlayerManager::setGlobalPlayState);
    connect(m_layoutManager, &PlayerLayoutManager::setGlobalMuteStateRequested, this, &GlobalPlayerManager::setGlobalMuteState);
    connect(m_layoutManager, &PlayerLayoutManager::setGlobalZoomStateRequested, this, &GlobalPlayerManager::setGlobalZoomState);

    connect(m_layoutManager, &PlayerLayoutManager::disableNavPanelRequested, this, &GlobalPlayerManager::disableNavPanelRequested);
    connect(m_layoutManager, &PlayerLayoutManager::enableNavPanelRequested, this, &GlobalPlayerManager::enableNavPanelRequested);
    connect(m_layoutManager, &PlayerLayoutManager::activePlayersMediaStateChanged, this, &GlobalPlayerManager::activePlayersMediaStateChanged);
    connect(m_layoutManager, &PlayerLayoutManager::activePlayersCountChanged, this, &GlobalPlayerManager::activePlayersCountChanged);
    
    connect(m_navPanel, &NavPanel::openMediaFileRequested, m_layoutManager, [this](const QString &filePath)
        { m_layoutManager->createLayoutFromPaths(QStringList(filePath)); qDebug() << "connexion russie " << filePath; }
    );
    
    connect(&ProjectManager::instance(), &ProjectManager::projectInitialized, this, &GlobalPlayerManager::createTimelineWidget);
    connect(&ProjectManager::instance(), &ProjectManager::projectDeleted, this, [this](){
        if(m_timeline) m_wasTimelineVisible = m_timeline->isVisible();
        disableSegmentation();
    });

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
/// @param player Si != nullptr, alors 1 player dans newPlayersWidget
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

    m_player = player;
    m_playersWidget = newPlayersWidget;

    // ajout du nouveau playerWidget et toolbar 
    if (newPlayersWidget){
        layout->addWidget(m_playersWidget);
    }

    if (newToolbar){
        m_toolbarWidget = newToolbar;
        auto *advancedToolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget);
        auto *globalToolbar = qobject_cast<GlobalToolbar*>(m_toolbarWidget);
        if(globalToolbar){
            if(m_separationLine){
                layout->addWidget(m_separationLine);
                m_separationLine->show();
            }
            m_thumbnailWorker->releaseOpenCvCap(); // plus besoin de thumbnail worker quand plus de 1 player, on release pour economiser de la mémoire
        }
        else if(advancedToolbar){
            if(m_separationLine){
                layout->removeWidget(m_separationLine);
                m_separationLine->hide();
            }

            connect(&ProjectManager::instance(), &ProjectManager::ejectMedia, advancedToolbar, &AdvancedToolbar::ejectRequest);

            connect(advancedToolbar, &AdvancedToolbar::previousMediaRequested, this, &GlobalPlayerManager::playPreviousMedia);
            connect(advancedToolbar, &AdvancedToolbar::nextMediaRequested, this, &GlobalPlayerManager::playNextMedia);
            disconnect(m_navPanel, &NavPanel::ejectCurrentMedia, nullptr, nullptr);
            connect(m_navPanel, &NavPanel::disableToolbarLoopRequested, advancedToolbar, &AdvancedToolbar::disableLoopMode);
            connect(m_navPanel, &NavPanel::ejectCurrentMedia, m_player, &PlayerWidget::eject);
            connect(advancedToolbar, &AdvancedToolbar::ejectUiUpdateDone, m_thumbnailWorker, &ThumbnailWorker::releaseOpenCvCap); // release la cap opencv après avoir fini d'ejecter 

            connect(advancedToolbar, &AdvancedToolbar::enableSegmentationRequest, this, &GlobalPlayerManager::enableSegmentation);
            connect(advancedToolbar, &AdvancedToolbar::disableSegmentationRequest, this, &GlobalPlayerManager::disableSegmentation);

        }

        layout->addWidget(m_toolbarWidget);
        m_toolbarWidget->setTBParent(this); // since toolbar was created in playerlayoutmanager, need to update its internal m_parent 
    }

    // maintient la fenetre dans l'écran actuel
    QTimer::singleShot(10, this, [this](){
        if (window()->isMaximized() || window()->isFullScreen())
            return;

        QWidget* win = window();
        QScreen* scr = QGuiApplication::screenAt(win->geometry().center());
        if (!scr) scr = QGuiApplication::primaryScreen();

        QRect available = scr->availableGeometry();
        QRect frame = win->frameGeometry();

        // clamp la taille si dépasse l'écran
        int w = qMin(frame.width(),  available.width());
        int h = qMin(frame.height(), available.height());

        // clamp pos
        int maxX = qMax(available.left(), available.right()  - w);
        int maxY = qMax(available.top(),  available.bottom() - h);

        int x = qBound(available.left(), frame.left(), maxX);
        int y = qBound(available.top(),  frame.top(),  maxY);

        win->move(x, y);
        if (frame.width() > available.width() || frame.height() > available.height())  win->resize(w, h);
    });
    
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

void GlobalPlayerManager::toggleNavPanel()
{

    if(m_navPanel->isOpen())
        closeNavPanel();
    else
        openNavPanel();
}
// slots

void GlobalPlayerManager::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(m_toolbarWidget && m_toolbarWidget->fullscreenBtn()->isChecked()){
        m_toolbarWidget->updateFullscreenPosition();
        m_toolbarWidget->adjustSize();
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

/// @brief Met à jour l'état du bouton zoom
/// @param state
void GlobalPlayerManager::setGlobalZoomState(bool state)
{
    if(m_toolbarWidget){
        m_toolbarWidget->zoomBtn()->setButtonState(state);
    }
}

/// @brief Cache la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::enableFullscreenPlayer()
{
    if(m_toolbarWidget)
        m_toolbarWidget->hide();
    if(m_separationLine){
        layout->removeWidget(m_separationLine);
        m_separationLine->hide();
    }

    emit enableFullscreenMainRequested();
}

/// @brief Affiche la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::disableFullscreenPlayer()
{
    if(m_toolbarWidget) {
        layout->removeWidget(m_separationLine);
        layout->removeWidget(m_toolbarWidget);

        m_toolbarWidget->setDefaultUI();

        layout->insertWidget(1, m_separationLine);
        layout->insertWidget(2, m_toolbarWidget);

        auto *advancedToolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget);
        if(!advancedToolbar){
            m_separationLine->show();
        }
        
        m_toolbarWidget->show();
        m_toolbarWidget->fullscreenBtn()->setButtonState(false);
    }

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

    auto* toolbar = qobject_cast<AdvancedToolbar*>(m_toolbarWidget);
    
    if(!toolbar) {
        qDebug() << "[GlobalPlayerManager] La toolbar n'est pas une AdvancedToolbar lors de la création de la timeline, la timeline n'est pas créée";
        return;
    }

    if(m_timeline){
        m_timeline->deleteLater();
        m_timeline = nullptr;
    }
    ProjectManager& projManager = ProjectManager::instance();
    Project* proj = projManager.projet();
    Media* projMedia = proj->media;

    if( ! projMedia ){
        qCritical() << "[GlobalPlayerManager] Pas de media dans le projet, impossible de créer une timeline";
        return;
    }

    if(projMedia->fps() == 0){
        projMedia->setFps(1);
        qDebug() << "[GlobalPlayerManager] FPS == 0, utilisation de FPS = 1";
    }

    if(projMedia->duration() == 0){
        qCritical() << "[GlobalPlayerManager] Pas de duréer pour le média, impossible de créer une timeline";
        return;
    }


    m_timeline = new TimelineWidget(m_thumbnailWorker, projMedia, m_player, proj->shots, this, width());
    m_timeline->setFixedHeight(160);
    m_timeline->hide();

    projManager.setTimeline(m_timeline);

    connect(m_player, &PlayerWidget::vlcTimeChanged, m_timeline, &TimelineWidget::updateCursorPos);

    connect(m_timeline, &TimelineWidget::saveNeeded, &projManager, &ProjectManager::setSaveNeeded);

    connect(m_timeline, &TimelineWidget::timelineSetPosition, m_player, &PlayerWidget::setTime);

    connect(m_timeline, &TimelineWidget::updateShotDetailRequest, m_navPanel, &NavPanel::timelineWidgetUpdateShotDetail);
    // Pour initialiser les informations dans le ShotDetail
    m_timeline->initShotDetail();
    connect(m_timeline, &TimelineWidget::disableTimeRelatedUI, m_navPanel, &NavPanel::disableShotControlButtons );
    connect(m_timeline, &TimelineWidget::enableTimeRelatedUI, m_navPanel, &NavPanel::enableShotControlButtons );
    connect(m_navPanel, &NavPanel::goToShotRequest, m_timeline, &TimelineWidget::goToShot);

    connect(m_timeline, &TimelineWidget::timelineSliderPositionRequested, toolbar, &SimpleToolbar::updateSliderValue );
    connect(m_timeline, &TimelineWidget::disableTimeRelatedUI, toolbar, &AdvancedToolbar::disableSlider );
    connect(m_timeline, &TimelineWidget::enableTimeRelatedUI, toolbar, &AdvancedToolbar::enableSlider );

    connect(toolbar, &AdvancedToolbar::toolbarCursorPositionRequested, m_timeline, &TimelineWidget::updateCursorVisually);
    layout->setSpacing(1);

    layout->addWidget(m_timeline);
    if(m_wasTimelineVisible) toolbar->getExtendedToolbar()->getSegmBtn()->click();
}

void GlobalPlayerManager::showAllToolbars(bool visible) {
    if (m_toolbarWidget) {
        visible ? m_toolbarWidget->showAnimation() : m_toolbarWidget->hideAnimation();
    }
    if (m_layoutManager) {
        m_layoutManager->showAllActivePlayersToolbars(visible);
    }
}


bool GlobalPlayerManager::isMouseOverAnyToolbar() const {
    if (m_toolbarWidget && m_toolbarWidget->rect().contains(m_toolbarWidget->mapFromGlobal(QCursor::pos()))) {
        return true;
    }

    if (m_layoutManager) {
        for (PlayerWidget* IPlayer : m_layoutManager->activePlayers()) { 
            if (IPlayer && IPlayer->toolbar()) {
                auto* toolbar = IPlayer->toolbar();
                if (toolbar->rect().contains(toolbar->mapFromGlobal(QCursor::pos()))) {
                    return true;
                }
            }
        }
    }

    return false;
}