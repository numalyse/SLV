#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    m_layoutManager = new PlayerLayoutManager();
    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);

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
