#include "Toolbars/GlobalToolbar.h"

#include "TextManager.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


GlobalToolbar::GlobalToolbar(QWidget *parent) : Toolbar(parent)
{
    QHBoxLayout* buttonLayout =  new QHBoxLayout();

    setDefaultUI();
}

void GlobalToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    // Créer un layout quand on est en fullscreen
}

void GlobalToolbar::setDefaultUI()
{

    if (layout() != nullptr) {
        delete layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    QHBoxLayout* buttonLayout = new QHBoxLayout();   
    buttonLayout->addWidget(m_playPauseBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    mainLayout->addLayout(buttonLayout);
}

