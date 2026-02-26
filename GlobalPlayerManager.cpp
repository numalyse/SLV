#include "GlobalPlayerManager.h"
#include <qlayout.h>

GlobalPlayerManager::GlobalPlayerManager(QWidget *parent)
    : QWidget{parent}

{
    m_layoutManager = new PlayerLayoutManager();
    setPlayers();
}

void GlobalPlayerManager::setPlayers()
{
    layout = new QVBoxLayout();

    layout->removeWidget(m_currentLayout);
    delete m_currentLayout;
    m_currentLayout = nullptr;

    m_currentLayout = m_layoutManager->createLayout(1);
    if (m_currentLayout){
        layout->addWidget(m_currentLayout);
        this->setLayout(layout);
    }
}

void GlobalPlayerManager::setPlayersFromPaths(QStringList filesPaths)
{
    qDebug() << "Current Layout : " << m_currentLayout;

    layout->removeWidget(m_currentLayout);
    qDebug() << "Allox";
    m_currentLayout = m_layoutManager->createLayoutFromPaths(filesPaths);
    layout->addWidget(m_currentLayout);
}
