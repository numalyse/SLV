#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "MediaWidget.h"
#include "CompositionWidget.h"
#include "Toolbars/SimpleToolbar.h"

#include <QWidget>
#include <qmainwindow.h>
#include <vlc/vlc.h>
#include <QSize>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    // ~PlayerWidget();

    void setActive(bool active);
    bool setMediaFromPath(const QString& filePath);

    SimpleToolbar* toolbar() {return m_toolBar;};
    MediaWidget* mediaWidget() { return m_mediaWidget; };
    double mediaFps() { return m_media_fps; };
    bool playing(){ return m_playing; };
    bool muted() { return m_muted; };

public slots:
    void play();
    void playFromAdvanced();
    void pause();
    void stop();
    void eject();
    void enablePlayerFullscreen();
    void disablePlayerFullscreen();
    void mute();
    void unmute();
    void setVolume(const int &vol);
    void takeScreenshot();
    void setSpeed(const unsigned int &speed);
    void setTime(int64_t time);
    void moveTimeBackward();
    void moveTimeForward();
    void updateFpsRequest(double);
    void enableLoopMode();
    void disableLoopMode();
    void startRecord();
    void endRecord();
    void rotate();
    void setOverlayMode(OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked);
    void onMediaSizeChanged(const QSize &size);
    void widgetSizeMove();
    bool event(QEvent *event);

    void enableButtons();
    void disableButtons();

signals:
    void addPlayerRequest();
    void removePlayerRequest(PlayerWidget* toBeRemoved);
    void duplicatePlayerRequest(PlayerWidget* toBeDuplicated);
    void enablePlayerFullscreenRequested(PlayerWidget* self);
    void disablePlayerFullscreenRequested(PlayerWidget* self);
    void updateSliderRangeRequest(int64_t);
    void vlcTimeChanged(int64_t);
    void updateFpsRequested(float);
    void setPlayUIRequested();
    void setPauseUIRequested();
    void checkPlayersPlayStatusRequested();
    void checkPlayersMuteStatusRequested();

    void playUiUpdateRequested();
    void pauseUiUpdateRequested();
    void stopUiUpdateRequested();
    void ejectUiUpdateRequested();
    void muteUiUpdateRequested();
    void unmuteUiUpdateRequested();
    void enableLoopUiUpdateRequested();
    void disableLoopUiUpdateRequested();
    void nameUiUpdateRequest(const QString& );

    void mediaPlayerLoaded();
    void mediaPlayerEjected();

    void mediaDropped(const QStringList&);
    void mediaSizeChanged(const QSize &size);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    double m_media_fps {};
    bool m_playing = false;
    bool m_muted = false;
    SimpleToolbar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QAction* m_addPlayerAction;
    QAction* m_removePlayerAction;
    MediaWidget* m_mediaWidget = nullptr;
    CompositionWidget* m_compositionWidget = nullptr;
    QSize m_mediaSize;

};

#endif // PLAYERWIDGET_H
