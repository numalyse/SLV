#include "NavPanel.h"
#include <QLabel.h>

NavPanel::NavPanel(QWidget *parent)
    : QWidget{parent}
{
    m_mainLayout = new QHBoxLayout(this);
    m_playlistWidget = new Playlist(this);
    m_sideWidget = m_playlistWidget;
    m_mainLayout->addWidget(m_sideWidget);
    m_mainLayout->setSpacing(0);
    m_sideWidget->hide();
    setFixedWidth(0);

    connect(m_playlistWidget, &Playlist::openMediaFileRequested, this, &NavPanel::openMediaFileRequested);
}

void NavPanel::showPanel()
{
    qDebug() << "bouton cliqué";
    m_sideWidget->show();
    setFixedWidth(300);
}

void NavPanel::hidePanel()
{
    m_sideWidget->hide();
    setFixedWidth(0);
}

// Fichiers chargés depuis le menuBar de mainWindow
void NavPanel::setPlaylistNewItem()
{

}
