#include "NavPanel.h"
#include <QLabel.h>
#include "SignalManager.h"

NavPanel::NavPanel(QWidget *parent)
    : QWidget{parent}
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);

    m_sideWidget = new QStackedWidget(this);

    m_playlistWidget = new Playlist(this);
    m_shotDetail = new ShotDetail(this);

    m_sideWidget->addWidget(m_playlistWidget); 
    m_sideWidget->addWidget(m_shotDetail);

    m_sideWidget->setCurrentWidget(m_playlistWidget);
    m_mainLayout->addWidget(m_sideWidget);

    m_sideWidget->hide();
    setFixedWidth(0);

    connect(m_playlistWidget, &Playlist::openMediaFileRequested, this, &NavPanel::openMediaFileRequested);
    connect(m_playlistWidget, &Playlist::disableToolbarLoopRequested, this, &NavPanel::disableToolbarLoopRequested);
    
    connect(m_shotDetail, &ShotDetail::goToShotRequested, this, &NavPanel::goToShotRequest);

    connect(&SignalManager::instance(), &SignalManager::extensionToolbarDisplayShotDetail, this, &NavPanel::displayShotDetail);
    connect(&SignalManager::instance(), &SignalManager::displayPlaylist, this, &NavPanel::displayPlaylist);

    m_thumbnailWorker = new ThumbnailWorker(this);
    connect(m_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &NavPanel::updateThumbnail);

    //connect(m_playlistWidget, &Playlist::updateImageRequested, this, &NavPanel::updateImageRequest);
    connect(m_shotDetail, &ShotDetail::updateImageRequested, this, &NavPanel::updateImageRequest);

    m_thumbnailWorker->start();

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

void NavPanel::playPreviousMedia()
{
    m_playlistWidget->playPreviousMedia();
}

void NavPanel::playNextMedia()
{
    m_playlistWidget->playNextMedia();
}

void NavPanel::displayShotDetail()
{
    m_sideWidget->setCurrentWidget(m_shotDetail);
}

void NavPanel::displayPlaylist()
{
    m_sideWidget->setCurrentWidget(m_playlistWidget);
}

void NavPanel::timelineWidgetUpdateShotDetail(int shotCount, int shotId, Shot * shot)
{
    m_shotDetail->updateShotDetail(shotCount, shotId, shot);
}

void NavPanel::disableShotControlButtons()
{
    m_shotDetail->toggleShotControlButtons(false);
}

void NavPanel::enableShotControlButtons()
{
    m_shotDetail->toggleShotControlButtons(true);
}

void NavPanel::updateImageRequest(int idShot, int64_t time, int64_t length, const QString& mediaPath, const QSize& targetSize){
    m_thumbnailWorker->requestThumbnail(idShot, time, length, mediaPath, targetSize);
}

void NavPanel::updateThumbnail(int shotId, QImage image){
    if (shotId == -1){
        m_shotDetail->updateTagImage(image);
    }else{
        //m_playlistWidget->updateThumbnail(shotId, image);
    }
}