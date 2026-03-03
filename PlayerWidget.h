#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "MediaWidget.h"
#include "Toolbars/SimpleToolbar.h"
#include "Toolbars/Toolbar.h"
#include "Media.h"

#include <QWidget>
#include <qmainwindow.h>
#include <vlc/vlc.h>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    // ~PlayerWidget();

    void setActive(bool active);
    void setMediaFromPath(const QString& filePath);

    SimpleToolbar* getToolbar() {return m_toolBar;};
    double getMediaFps() { return m_media_fps; };
    bool getIsPlaying(){ return m_isPlaying; };

public slots:
    void play();
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
    void updateFpsRequest(double);

signals:
    void addPlayerRequest();
    void removePlayerRequest(PlayerWidget* toBeRemoved);
    void enablePlayerFullscreenRequested(PlayerWidget* self);
    void disablePlayerFullscreenRequested(PlayerWidget* self);
    void updateSliderRangeRequest(int64_t);
    void updateSliderValueRequest(int64_t);
    void updateFpsRequested(float);
    void setPlayUIRequested();
    void setPauseUIRequested();
    void checkPlayersStatusRequested();

private:
    Media *m_media = nullptr;
    double m_media_fps = 0.0;
    bool m_isPlaying = false;
    SimpleToolbar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QAction* m_addPlayerAction;
    QAction* m_removePlayerAction;
    MediaWidget* m_mediaWidget = nullptr;

};

#endif // PLAYERWIDGET_H
