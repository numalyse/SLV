#include "GlobalPlayerManager.h"

#include "Toolbars/Toolbar.h"
#include "Toolbars/AdvancedToolbar.h"

#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    m_layoutManager = new PlayerLayoutManager();
    setPlayers();
    connect(m_layoutManager, &PlayerLayoutManager::enableFullscreenGlobalRequested, this, &GlobalPlayerManager::enableFullscreenGlobal);
    connect(m_layoutManager, &PlayerLayoutManager::disableFullscreenGlobalRequested, this, &GlobalPlayerManager::disableFullscreenGlobal);
}

void GlobalPlayerManager::setPlayers()
{
    layout = new QVBoxLayout();

    layout->removeWidget(m_playersWidget);
    delete m_playersWidget;
    m_playersWidget = nullptr;

    m_playersWidget = m_layoutManager->createLayout(1);
    if (m_playersWidget){
        layout->addWidget(m_playersWidget);
        this->setLayout(layout);
    }
}

void GlobalPlayerManager::setPlayersFromPaths(QStringList filesPaths)
{
    qDebug() << "Current Layout : " << m_playersWidget;

    layout->removeWidget(m_playersWidget);
    qDebug() << "Allox";
    m_playersWidget = m_layoutManager->createLayoutFromPaths(filesPaths);
    layout->addWidget(m_playersWidget);
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
