#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include "Media.h"

#include <vlc/vlc.h>
#include <QWidget>
#include <SignalManager.h>
#include <QFrame>

class MediaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MediaWidget(QWidget *parent = nullptr);

    ~MediaWidget();
    void managePlayerSystem();
    void setActive(bool active);
    bool setMediaFromPath(const QString& filePath);

    libvlc_media_player_t *m_player = nullptr;

    Media* media(){ return m_media;};

public slots:
    bool play();
    bool pause();
    bool stop();
    bool eject();
    bool mute();
    bool unmute();
    void togglePlayPause();
    void setVolume(const int &vol);
    void setSpeed(const unsigned int &speedIndex);
    void takeScreenshot();
    void setTime(int64_t);
    void enableLoopMode();
    void disableLoopMode();
    void hideMedia();
    void showMedia();
    void startRecord();
    void endRecord();

private:


    bool m_loopActivated = true;
    const float m_speedSteps[7] = {0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    libvlc_event_manager_t* m_eventManager = nullptr;
    libvlc_event_manager_t* m_parseEventManager = nullptr;
    Media* m_media = nullptr;
    QWidget* m_mediaSurface = nullptr;
    QFrame* m_blackFrame = nullptr;
    int m_startRecordTime = -1;

    static void onVlcEvent(const libvlc_event_t* event, void* userData);
    

    void createEventManager();
    void createMedia(const QString& filePath);

    void releaseMedia();
    void releaseEventManager();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void vlcTimeChanged(int64_t newTime);
    void activated(MediaWidget* self);
    void updateSliderRangeRequested(int64_t mediaDuration);
    void updateFpsRequested(double fps);
    void mediaPlayerLoaded();
    void mediaPlayerEjected();
    void nameUiUpdateRequested(const QString&);
    void mediaFinished();
    void volumeChanged(const QString&);
    void speedChanged(const QString&);
};

#endif // MEDIAWIDGET_H
