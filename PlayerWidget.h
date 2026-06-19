#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "MediaWidget.h"
#include "BlackOpacityWidget.h"
#include "CompositionWidget.h"
#include "DrawingWidget.h"
#include "Toolbars/SimpleToolbar.h"
#include "ExtractSequenceWidget.h"
#include "SignalManager.h"
#include "MediaLogoWidget.h"

#include <QMessageBox>
#include <QWidget>
#include <QMainWindow>
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
    QString getMediaPath();
    int getCurrentTime();
    double getSar();
    double mediaFps() { return m_media_fps; };
    bool playing(){ return m_playing; };
    bool muted() { return m_muted; };
    bool zoomed() { return m_toolBar->zoomBtn()->isChecked(); }

public slots:
    void play();
    void playFromAdvanced();
    void pause();
    void togglePlayPause(bool isPlaying);
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
    void enableZoomMode();
    void disableZoomMode();
    void startRecord();
    void endRecord();
    void rotate();
    void setBlackOpacityMode(bool isShown, double opacity);
    void showDrawingMode(bool isEnabled);
    void setOverlayMode(bool showOverlay, OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked);
    void onMediaRectChanged(const QRect &rect);
    void widgetSizeChange();
    bool event(QEvent *event) override;
    void openSequenceExtractionDialog();

    void enableButtons();
    void disableButtons();
    void mediaPlayerEjectedHandler();

    /// @brief Reset compositionWidget, drawingWidget and blackOpacityWidget
    void resetLayerWidgets();

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
    void enableZoomUiUpdateRequested();
    void disableZoomUiUpdateRequested();
    void nameUiUpdateRequest(const QString& );
    void enableFullscreenUiUpdateRequested();
    void disableFullscreenUiUpdateRequested();

    void mediaPlayerLoaded();
    void mediaPlayerEjected();

    void mediaDropped(const QStringList&);
    void mediaRectChanged(const QRect &rect);

    void subtitleTrackAdded(int trackId, const QString& label); 


protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void restoreOverlayStackOrder();

    void updateSingleOverlayGeom(QWidget *widget, bool isVisible);
    void updateSingleLogoGeom(QWidget* widget, bool isVisible);

    double m_media_fps {};
    bool m_playing = false;
    bool m_muted = false;
    SimpleToolbar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QAction* m_addPlayerAction;
    QAction* m_removePlayerAction;
    MediaWidget* m_mediaWidget = nullptr;
    BlackOpacityWidget* m_blackOpacityWidget = nullptr;
    CompositionWidget* m_compositionWidget = nullptr;
    DrawingWidget* m_drawingWidget = nullptr;
    MediaLogoWidget* m_audioLogoWidget = nullptr;
    // separate widget used for the drag and drop logo to prevent UI flickering 
    // issues caused by having two labels inside a single widget.
    MediaLogoWidget* m_dragDropLogoWidget = nullptr; 
    QSize m_mediaSize;
    QRect m_mediaRect;
    QString m_pendingFilePath;
};

#endif // PLAYERWIDGET_H
