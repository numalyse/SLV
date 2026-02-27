#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    layout = new QVBoxLayout(this);
    m_layoutManager = new PlayerLayoutManager();
    connect(m_layoutManager, &PlayerLayoutManager::updateContainerRequest, this, &GlobalPlayerManager::updateContainer);

    m_layoutManager->createLayout(1);
}

void GlobalPlayerManager::setPlayersFromPaths(QStringList filesPaths)
{
    m_layoutManager->createLayoutFromPaths(filesPaths);
}

void GlobalPlayerManager::updateContainer(int videoPlayersCount, QWidget * newPlayersWidget, Toolbar* newToolbar)
{
    // clean ancienne UI
    if (m_playersWidget) {
        layout->removeWidget(m_playersWidget);
        m_playersWidget->deleteLater(); 
    }
    if (m_toolbarWidget){
        layout->removeWidget(m_toolbarWidget);
        m_toolbarWidget->deleteLater(); 
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

}
