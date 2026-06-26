#include "NavPanel.h"
#include <QLabel>
#include "SignalManager.h"
#include <QScrollArea>

NavPanel::NavPanel(ThumbnailWorker* thumbnailWorker, QWidget *parent)
    : QWidget{parent}, p_thumbnailWorker{thumbnailWorker}
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);
    
    m_sideWidget = new QStackedWidget(this);

    m_playlistWidget = new Playlist(this);
    m_shotDetail = new ShotDetail(this);
    m_sideWidget->addWidget(m_playlistWidget); 
    m_sideWidget->addWidget(m_shotDetail);
    m_sideWidget->setCurrentWidget(m_playlistWidget);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_scrollArea->setWidget(m_sideWidget);
    m_mainLayout->addWidget(m_scrollArea);

    m_sideWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_sideWidget->hide();
    setFixedWidth(0);

    connect(m_playlistWidget, &Playlist::openMediaFileRequested, this, &NavPanel::openMediaFileRequested);
    connect(m_playlistWidget, &Playlist::disableToolbarLoopRequested, this, &NavPanel::disableToolbarLoopRequested);
    connect(m_playlistWidget, &Playlist::ejectCurrentMedia, this, &NavPanel::ejectCurrentMedia);
    
    connect(m_shotDetail, &ShotDetail::goToShotRequested, this, &NavPanel::goToShotRequest);

    connect(&SignalManager::instance(), &SignalManager::extensionToolbarDisplayShotDetail, this, &NavPanel::displayShotDetail);
    connect(&SignalManager::instance(), &SignalManager::displayPlaylist, this, &NavPanel::displayPlaylist);

    connect(p_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &NavPanel::updateThumbnail);

    connect(m_shotDetail, &ShotDetail::updateImageRequested, this, &NavPanel::updateImageRequest);
    connect(m_shotDetail, &ShotDetail::clearThumbnailQueueRequested, this, [this](){
        p_thumbnailWorker->clearPriotityQueue(); // prevent having many images inside the priority queue
    });
    
}

void NavPanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void NavPanel::showPanel()
{
    qDebug() << "bouton cliqué";
    m_isOpen = true;
    m_sideWidget->show();
    setFixedWidth(300);
}

void NavPanel::hidePanel()
{
    m_isOpen = false;
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

void NavPanel::timelineWidgetUpdateShotDetail(int shotCount, int requestId, Shot * shot)
{
    m_shotDetail->updateShotDetail(shotCount, requestId, shot);
}

void NavPanel::disableShotControlButtons()
{
    m_shotDetail->toggleShotControlButtons(false);
}

void NavPanel::enableShotControlButtons()
{
    m_shotDetail->toggleShotControlButtons(true);
}

void NavPanel::updateImageRequest(int requestId, int64_t time, int64_t length, Media* media, const QSize& targetSize){
    if(media->type() == MediaType::Video) p_thumbnailWorker->requestThumbnail(requestId, time, length, media->filePath(), targetSize, media->sar());
}

void NavPanel::updateThumbnail(int imageId, QImage image){
    if (imageId == -1){
        m_shotDetail->updateTagImage(image);
    }
}

