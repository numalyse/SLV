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

public slots:
    void play();
    void pause();
    void setTime(int64_t time);
    void updateFpsRequest(float);

signals:
    void addPlayerRequest();
    void removePlayerRequest(PlayerWidget* toBeRemoved);
    void enablePlayerFullscreenRequested(PlayerWidget* self);
    void disablePlayerFullscreenRequested(PlayerWidget* self);
    void updateSliderRangeRequest(int64_t);
    void updateSliderValueRequest(int64_t);
    void updateFpsRequested(float);

private:
    Media *m_media = nullptr;
    float m_media_fps = 0;
    SimpleToolbar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QAction* m_addPlayerAction;
    QAction* m_removePlayerAction;
    MediaWidget* m_mediaWidget = nullptr;

public slots:
    void enablePlayerFullscreen();
    void disablePlayerFullscreen();
};

#endif // PLAYERWIDGET_H
