#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include "Media.h"
#include "VideoCaptureManager.h"

#include <vlc/vlc.h>
#include <QWidget>
#include <SignalManager.h>
#include <QFrame>
#include <QDir>
#include <QComboBox>

class MediaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MediaWidget(QWidget *parent = nullptr);

    ~MediaWidget();
    void managePlayerSystem();
    void setActive(bool active);
    bool setMediaFromPath(const QString& filePath);

    libvlc_instance_t *m_vlcInstance = nullptr;
    libvlc_media_player_t *m_player = nullptr;

    Media* media(){ return m_media;};
    int getCurrentTime(){ return libvlc_media_player_get_time(m_player); }

    QList<QPair<int, QString>> audioTracks() const;
    QList<QPair<int, QString>> subtitlesTracks() const;

    void getAudioTracks();
    void getSubtitlesTracks();

public slots:
    bool play();
    bool pause();
    bool stop();
    bool eject();
    void parseTracks();
    void setAudioTrack(int trackId);
    void setSubtitleTrack(int trackId);
    bool mute();
    bool unmute();
    void togglePlayPause();
    void setVolume(const int &vol);
    void setSpeed(const unsigned int &speedIndex);
    void takeScreenshot();
    void setTime(int64_t);
    void moveTimeBackward();
    void moveTimeForward();
    void enableLoopMode();
    void disableLoopMode();
    void hideMedia();
    void showMedia();
    void startRecord();
    void endRecord();
    void rotate();
    void hFlip();
    void vFlip();

    QPoint getMediaPosRect() const;
    QRect getMediaDisplayRect() const;

    void updateTracks();

private:
    QSize m_mediaSize;

    bool m_loopActivated = true;
    bool m_vflipped = false;
    bool m_hflipped = false;
    const float m_speedSteps[7] = {0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    unsigned int m_rotationIndex = 0;
    std::vector<const char*> m_vlcArgs ={"--quiet",
        "--aout=directsound",
        "--no-video-title-show",
        "--no-input-fast-seek"};
    inline static const char* m_rotationSteps[4] = {"--rotate-angle=0", "--rotate-angle=90", "--rotate-angle=180", "--rotate-angle=270"};
    inline static const char* m_flipSteps[2] = {"--transform-type=hflip", "-transform-type=vflip"};
    libvlc_event_manager_t* m_eventManager = nullptr;
    libvlc_event_manager_t* m_parseEventManager = nullptr;
    Media* m_media = nullptr;
    QWidget* m_mediaSurface = nullptr;
    QFrame* m_blackFrame = nullptr;
    int m_startRecordTime = -1;
    VideoCaptureManager m_videoCaptureManager;

    static void onVlcEvent(const libvlc_event_t* event, void* userData);

    /// @brief Recreates a vlc instance to apply transformation on media
    void transformMedia();

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
    void mediaRectChanged(const QRect &rect);
    void updateAudioTracksRequested(const QList<QPair<int, QString>>& tracks);
    void updateSubtitlesTracksRequested(const QList<QPair<int, QString>>& tracks);
    void setAudioTrackDefaultRequested();
    void setSubtitlesTrackDefaultRequested();
    void hFlipUiUpdateRequested();
    void vFlipUiUpdateRequested();

};

#endif // MEDIAWIDGET_H
