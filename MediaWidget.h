#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

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
    void setMediaFromPath(const QString& filePath);



public slots:
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void eject();
    void mute();
    void unmute();
    void setVolume(const int &vol);
    void setSpeed(const unsigned int &speedIndex);
    void takeScreenshot();
    void setTime(int64_t);
    void enableLoopMode();
    void disableLoopMode();
    void hideMedia();
    void showMedia();

private:
    libvlc_instance_t *m_vlc = nullptr;
    libvlc_media_player_t *m_player = nullptr;
    bool m_loopActivated = true;
    const float m_speedSteps[7] = {0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    libvlc_event_manager_t* m_eventManager = nullptr;
    libvlc_event_manager_t* m_parseEventManager = nullptr;
    Media* m_media = nullptr;
    QFrame* m_blackFrame = nullptr;

    static void onVlcEvent(const libvlc_event_t* event, void* userData);
    static QMap<libvlc_meta_t, QString> getMetaParsedMedia(libvlc_media_t *parsedMedia);
    static double getFpsParsedMedia(libvlc_media_t *parsedMedia);
    
    void releaseMedia();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void updateSliderValueRequested(int64_t newTime);
    void activated(MediaWidget* self);
    void updateSliderRangeRequested(int64_t mediaDuration);
    void updateFpsRequested(double fps);
};

#endif // MEDIAWIDGET_H
