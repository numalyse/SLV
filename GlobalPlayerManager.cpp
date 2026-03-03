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
    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, &GlobalPlayerManager::enableFullscreenGlobal);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, &GlobalPlayerManager::disableFullscreenGlobal);
    connect(m_layoutManager, &PlayerLayoutManager::setGlobalPlayStateRequested, this, &GlobalPlayerManager::setGlobalPlayState);

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
void GlobalPlayerManager::updateContainer(int videoPlayersCount, QWidget * newPlayersWidget, Toolbar* newToolbar)
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
}

/// @brief Met à jour l'état du bouton play pause 
/// @param state 
void GlobalPlayerManager::setGlobalPlayState(bool state)
{
    if(m_toolbarWidget){
        m_toolbarWidget->getPlayPauseBtn()->setButtonState(state);
    }
}

/// @brief Cache la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::enableFullscreenGlobal()
{
    if(m_toolbarWidget)
        m_toolbarWidget->hide();
    emit enableFullscreenMainRequested();
}

/// @brief Affiche la toolbar si elle est présente et envoie un signal à la mainWindow
void GlobalPlayerManager::disableFullscreenGlobal()
{
    if(m_toolbarWidget)
        m_toolbarWidget->show();
    emit disableFullscreenMainRequested();
}
