#include "PlayerWidget.h"
#include "Toolbars/SimpleToolbar.h"
#include "ProjectManager.h"
#include "SignalManager.h"
#include "CompositionWidget.h"

#include <QDebug>
#include <QApplication>
#include <QResizeEvent>
#include <QShowEvent>
#include <QFile>
#include <QKeyEvent>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QStackedLayout>


PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    //setMinimumSize(640, 360);
    //resize(800, 450);

    // setAttribute(Qt::WA_NativeWindow);
    // setAttribute(Qt::WA_DontCreateNativeAncestors);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAcceptDrops(true);

    // ===== Toolbar ===== //
    m_toolBar = new SimpleToolbar(this);

    connect(m_toolBar, &SimpleToolbar::removePlayerRequest, this, [this]() {
        emit removePlayerRequest(this);
    });

    connect(m_toolBar, &SimpleToolbar::duplicatePlayerRequested, this, [this](){
        emit duplicatePlayerRequest(this);
    });

    m_mediaWidget = new MediaWidget(this);

    connect(m_toolBar, &Toolbar::playRequest, this, &PlayerWidget::play);
    connect(m_toolBar, &Toolbar::pauseRequest, this, &PlayerWidget::pause);
    connect(m_toolBar, &Toolbar::stopRequest, this, &PlayerWidget::stop);
    connect(m_toolBar, &Toolbar::ejectRequest, this, &PlayerWidget::eject);
    connect(m_toolBar, &Toolbar::enableFullscreenRequest, this, &PlayerWidget::enablePlayerFullscreen);
    connect(m_toolBar, &Toolbar::disableFullscreenRequest, this, &PlayerWidget::disablePlayerFullscreen);
    connect(m_toolBar, &SimpleToolbar::enableMuteRequest, this, &PlayerWidget::mute);
    connect(m_toolBar, &SimpleToolbar::disableMuteRequest, this, &PlayerWidget::unmute);
    connect(m_toolBar, &SimpleToolbar::volumeChanged, m_mediaWidget, &MediaWidget::setVolume);
    connect(m_toolBar, &SimpleToolbar::speedChanged, m_mediaWidget, &MediaWidget::setSpeed);
    connect(m_toolBar, &Toolbar::screenshotRequest, m_mediaWidget, &MediaWidget::takeScreenshot);
    connect(m_toolBar, &SimpleToolbar::setPositionRequested, this, &PlayerWidget::setTime);
    connect(m_toolBar, &SimpleToolbar::enableLoopModeRequest, this, &PlayerWidget::enableLoopMode);
    connect(m_toolBar, &SimpleToolbar::disableLoopModeRequest, this, &PlayerWidget::disableLoopMode);

    connect(this, &PlayerWidget::playUiUpdateRequested, m_toolBar, &SimpleToolbar::playUiUpdate);
    connect(this, &PlayerWidget::pauseUiUpdateRequested, m_toolBar, &SimpleToolbar::pauseUiUpdate);
    connect(this, &PlayerWidget::muteUiUpdateRequested, m_toolBar, &SimpleToolbar::muteUiUpdate);
    connect(this, &PlayerWidget::unmuteUiUpdateRequested, m_toolBar, &SimpleToolbar::unmuteUiUpdate);
    connect(this, &PlayerWidget::ejectUiUpdateRequested, m_toolBar, &SimpleToolbar::ejectUiUpdate);
    connect(this, &PlayerWidget::stopUiUpdateRequested, m_toolBar, &SimpleToolbar::stopUiUpdate);
    connect(this, &PlayerWidget::enableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::enableLoopUiUpdate);
    connect(this, &PlayerWidget::disableLoopUiUpdateRequested, m_toolBar, &SimpleToolbar::disableLoopUiUpdate);
    connect(this, &PlayerWidget::nameUiUpdateRequest, m_toolBar, &SimpleToolbar::nameUiUpdate);
    connect(m_mediaWidget, &MediaWidget::volumeChanged, m_toolBar, &SimpleToolbar::volumeUiUpdate);
    connect(m_mediaWidget, &MediaWidget::speedChanged, m_toolBar, &SimpleToolbar::speedUiUpdate);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerLoaded, this, &PlayerWidget::enableButtons);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerEjected, this, &PlayerWidget::disableButtons);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerLoaded, this, &PlayerWidget::mediaPlayerLoaded);
    connect(m_mediaWidget, &MediaWidget::mediaPlayerEjected, this, &PlayerWidget::mediaPlayerEjected);

    connect(this, &PlayerWidget::mediaDropped, &SignalManager::instance(), &SignalManager::playerWidgetMediaDropped);

    QWidget* containerWidget = new QWidget(this);
    QStackedLayout* stack = new QStackedLayout(containerWidget);
    stack->setContentsMargins(0,0,0,0);
    stack->addWidget(m_mediaWidget);

    m_compositionWidget = new CompositionWidget(containerWidget);
    stack->setStackingMode(QStackedLayout::StackAll);
    stack->addWidget(m_compositionWidget);

    //m_compositionWidget->setOverlayMode(CompositionWidget::GoldenRatio);
    //m_compositionWidget->raise(); 

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(containerWidget);
    layout->addWidget(m_toolBar);

    connect(m_mediaWidget, &MediaWidget::updateSliderRangeRequested, this, &PlayerWidget::updateSliderRangeRequest);
    connect(m_mediaWidget, &MediaWidget::vlcTimeChanged, this, &PlayerWidget::vlcTimeChanged);
    connect(m_mediaWidget, &MediaWidget::updateFpsRequested, this, &PlayerWidget::updateFpsRequest);
    connect(m_mediaWidget, &MediaWidget::nameUiUpdateRequested, this, &PlayerWidget::nameUiUpdateRequest);

    connect(this, &PlayerWidget::updateSliderRangeRequest, m_toolBar, &SimpleToolbar::updateSliderRange);
    connect(this, &PlayerWidget::vlcTimeChanged, m_toolBar, &SimpleToolbar::updateSliderValue);
    connect(this, &PlayerWidget::updateFpsRequested, m_toolBar, &SimpleToolbar::updateFps);

    connect(&SignalManager::instance(), &SignalManager::timelineSetPosition, this, &PlayerWidget::setTime);

    connect(m_mediaWidget, &MediaWidget::mediaRectChanged, this, &PlayerWidget::onMediaRectChanged);
    connect(this, &PlayerWidget::mediaRectChanged, m_compositionWidget, &CompositionWidget::onMediaRectChanged);
    connect(&SignalManager::instance(), &SignalManager::windowMovedOrResized, this, &PlayerWidget::widgetSizeChange);

}

// PlayerWidget::~PlayerWidget()
// {
//     if (m_player) {
//         libvlc_media_player_stop(m_player);
//         libvlc_media_player_release(m_player);
//     }

//     if (m_vlc) {
//         libvlc_release(m_vlc);
//     }
// }


void PlayerWidget::setActive(bool active)
{
    setStyleSheet(active
                      ? "border: 2px solid #2a82da;"
                      : "border: none;");
}

bool PlayerWidget::setMediaFromPath(const QString& filePath)
{
    if (m_mediaWidget->setMediaFromPath(filePath)){
        m_playing = true;
        m_muted = false;
        emit playUiUpdateRequested();
        emit unmuteUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
        emit checkPlayersMuteStatusRequested();
        return true;
    }
    return false;
}

void PlayerWidget::enablePlayerFullscreen()
{
    emit enablePlayerFullscreenRequested(this);
}

void PlayerWidget::disablePlayerFullscreen()
{
    emit disablePlayerFullscreenRequested(this);
}

// slots 

/// @brief Play la video, si pas de media dans le player : créer un QFileDialog pour choisir un fichier à charger.
void PlayerWidget::play()
{
    if (m_mediaWidget->play()){
        m_playing = true;
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }else {
        QString file_path = QFileDialog::getOpenFileName(this, "Ouvrir un fichier multimédia", "/", "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)");
        if(file_path != ""){
            setMediaFromPath(file_path);
        }
        else
            emit m_toolBar->selectFilePlayCanceled();
    }
}

/// @brief Play la video connecté à une advanced toolbar, créer un projet en plus de l'action de base
void PlayerWidget::playFromAdvanced()
{
    if (m_mediaWidget->play()){
        m_playing = true;
        emit playUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }else {
        QString file_path = QFileDialog::getOpenFileName(this, "Ouvrir un fichier multimédia", "/", "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)");
        if(file_path != ""){
            if (setMediaFromPath(file_path)){
                ProjectManager::instance().createProject(m_mediaWidget->media());
            }
        }
        else
            emit m_toolBar->selectFilePlayCanceled();
    }
}

void PlayerWidget::pause()
{
    if (m_mediaWidget->pause()){
        m_playing = false;
        emit pauseUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    }

}

void PlayerWidget::stop()
{
    if(m_mediaWidget->stop()){
        m_playing = false;
        emit stopUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
    };

}

void PlayerWidget::eject()
{
    // TODO : demander à l'utilisateur s'il veut ejecter car cela va supprimer le project
    if(m_mediaWidget->eject()){
        m_playing = false;
        emit ejectUiUpdateRequested();
        emit checkPlayersPlayStatusRequested();
        emit SignalManager::instance().displayPlaylist();
        ProjectManager::instance().deleteProject();
    }

}

void PlayerWidget::mute()
{
    if(m_mediaWidget->mute()){
        m_muted = true;
        emit muteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }

}

void PlayerWidget::unmute()
{
    if(m_mediaWidget->unmute()){
        m_muted = false;
        emit unmuteUiUpdateRequested();
        emit checkPlayersMuteStatusRequested();
    }
}

void PlayerWidget::setVolume(const int &vol)
{
    m_mediaWidget->setVolume(vol);
}

void PlayerWidget::setSpeed(const unsigned int &speed)
{
    m_mediaWidget->setSpeed(speed);
}
void PlayerWidget::setTime(int64_t time){
    m_mediaWidget->setTime(time);
}

void PlayerWidget::moveTimeBackward(){
    m_mediaWidget->moveTimeBackward();
}

void PlayerWidget::moveTimeForward(){
    m_mediaWidget->moveTimeForward();
}

void PlayerWidget::updateFpsRequest(double newFps){
    m_media_fps = newFps;
    emit updateFpsRequested(newFps);
}

void PlayerWidget::takeScreenshot()
{
    m_mediaWidget->takeScreenshot();
}

void PlayerWidget::enableLoopMode()
{
    m_mediaWidget->enableLoopMode();
    emit enableLoopUiUpdateRequested();
}

void PlayerWidget::disableLoopMode()
{
    m_mediaWidget->disableLoopMode();
    emit disableLoopUiUpdateRequested();
}

void PlayerWidget::startRecord()
{
    m_mediaWidget->startRecord();
}

void PlayerWidget::endRecord()
{
    m_mediaWidget->endRecord();
}

void PlayerWidget::rotate()
{
    m_mediaWidget->rotate();
}

void PlayerWidget::setOverlayMode(OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked){

    m_compositionWidget->setOverlayMode(overlayMode, vFlipChecked, hFlipChecked);
}

void PlayerWidget::onMediaRectChanged(const QRect &rect)
{
    m_mediaRect = rect;
    emit mediaRectChanged(m_mediaRect);
    qDebug() << "PlayerWidget m_mediaRect : " << m_mediaRect;
}

// void PlayerWidget::widgetSizeChange()
// {
//     int parent_w = m_compositionWidget->parentWidget()->width();
//     int parent_h = m_compositionWidget->parentWidget()->height();

//     QPoint p = m_compositionWidget->parentWidget()->mapToGlobal(QPoint(0, 0));

//     int x = p.x();
//     int y = p.y();

//     if(parent_w > m_compositionWidget->width()){
//         int offset_x = (parent_w - m_compositionWidget->width());
//         x = x + offset_x;
//     }

//     if(parent_h > m_compositionWidget->height()){
//         int offset_y = (parent_h - m_compositionWidget->height());
//         y = y + offset_y;
//     }
//     m_compositionWidget->move(QPoint(x,y));
// }

void PlayerWidget::widgetSizeChange()
{
    if (!m_compositionWidget || !m_mediaWidget)
        return;

    QPoint globalPos = m_mediaWidget->mapToGlobal(QPoint(0, 0));

    int w = m_mediaWidget->width();
    int h = m_mediaWidget->height();

    m_compositionWidget->setGeometry(globalPos.x(), globalPos.y(), w, h);
}

bool PlayerWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
        m_compositionWidget->show();
        QTimer::singleShot(50, this, SLOT(widgetSizeChange())); 
        break;
    case QEvent::WindowActivate:
    case QEvent::Resize:
    case QEvent::Move:
        widgetSizeChange();
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void PlayerWidget::enableButtons()
{
    m_toolBar->enableButtons();
}

void PlayerWidget::disableButtons()
{
    m_toolBar->disableButtons();
}

void PlayerWidget::dragEnterEvent(QDragEnterEvent *event){
    if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
}

void PlayerWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        eject();

        QList<QUrl> urlList = mimeData->urls();

        for (const QUrl &url : urlList) {
            
            QString filePath = url.toLocalFile();
            qDebug() << "Fichier droppé :" << filePath;

            emit mediaDropped(QStringList(filePath));

        }
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void PlayerWidget::keyPressEvent(QKeyEvent *event)
{
    // if (event->key() == Qt::Key_Space) {
    //     togglePlayPause();
    // } else {
    //     QWidget::keyPressEvent(event);
    // }

    if (event->key() == Qt::LeftArrow) {
        moveTimeBackward();
    } else {
        QWidget::keyPressEvent(event);
    }

    if (event->key() == Qt::RightArrow) {
        moveTimeForward();
    } else {
        QWidget::keyPressEvent(event);
    }

    if (event->key() == Qt::Key_C) {
        takeScreenshot();
    } else {
        QWidget::keyPressEvent(event);
    }
}



