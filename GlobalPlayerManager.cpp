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
    auto *layout = new QVBoxLayout(this);

    layout->removeWidget(m_currentLayout);
    delete m_currentLayout;
    m_currentLayout = nullptr;

    m_currentLayout = m_layoutManager->createLayout();
    if (m_currentLayout){
        layout->addWidget(m_currentLayout);
        this->setLayout(layout);
    }
}
