#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    m_layoutManager = new PlayerLayoutManager();
    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);
    setPlayers();
    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, &GlobalPlayerManager::enableFullscreenGlobal);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, &GlobalPlayerManager::disableFullscreenGlobal);
}

    layout = new QVBoxLayout(this);

    m_playersWidget = m_layoutManager->createLayout(1);
    if (m_playersWidget){
        layout->addWidget(m_playersWidget);
    }
}

void GlobalPlayerManager::setPlayersFromPaths(QStringList filesPaths)
{
    layout->removeWidget(m_playersWidget);
    if (m_playersWidget) { 
        m_playersWidget->deleteLater();
    }
    m_playersWidget = m_layoutManager->createLayoutFromPaths(filesPaths);
    layout->addWidget(m_playersWidget);
}

void GlobalPlayerManager::updateContainer(int videoPlayersCount, QWidget * newPlayersWidget)
{

    if(videoPlayersCount){
        //mettre a jour barre d'outils ect.
    }
    else {
        // mettre a jour barre d'outils si necessaire, modifier ui ect.
    }

    layout->removeWidget(m_playersWidget);

    if (m_playersWidget) {

        m_playersWidget->deleteLater(); 
    }

    m_playersWidget = newPlayersWidget;
    if (m_playersWidget){
        layout->addWidget(newPlayersWidget);
    }
}

void GlobalPlayerManager::enableFullscreenGlobal()
{
    if(m_toolbarWidget)
        m_toolbarWidget->hide();
    emit enableFullscreenMainRequested();
}

void GlobalPlayerManager::disableFullscreenGlobal()
{
    if(m_toolbarWidget)
        m_toolbarWidget->show();
    emit disableFullscreenMainRequested();
}
