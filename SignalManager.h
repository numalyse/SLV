#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "Annotation.h"
#include "Shot.h"
#include <QObject>
#include <vlc/vlc.h>

enum class PanelType
{
    Playlist,
    ShotDetail,
    Annotation
};

enum PlayerLayoutArrangement
{
    Arrangement1,
    Arrangement2H,
    Arrangement2V,
    Arrangement3H,
    Arrangement3V,
    Arrangement3Top,
    Arrangement3Bot,
    Arrangement3Left,
    Arrangement3Right,
    Arrangement4,
    ArrangementUnknown
};

class SignalManager: public QObject
{
Q_OBJECT
public:

    static SignalManager &instance() {
        static SignalManager _instance;
        return _instance;
    };

private:
    SignalManager(QObject* parent = nullptr) : QObject(parent){};

signals:
    // window
    void windowMovedOrResized();

    // media
    void mediaWidgetMediaFinished();
    void mediaVolumeChanged(const QString&);
    void mediaSpeedChanged(const QString&);
    void newArrangementRequested(const PlayerLayoutArrangement&);
    void activateMediaChangeBtn(const bool);
    void updateMediaChangeButtons(bool canGoPrevious, bool canGoNext);

    // player 
    void playerWidgetSelectFileCanceled();
    void playerWidgetMediaDropped(const QStringList&);

    // panel
    void displayNavPanel(PanelType type);
    void toggleNavPanel(PanelType type);
    bool isNavelPanelOpen();
    PanelType typeCurrentPanel();

    // panel - playlist
    void displayPlaylist();
    void playlistEjectPlayer();
    void addPlaylistItems(const QStringList&);
    void requestPlaylistSize();
    void requestPlaylistNextMedia();
    void requestPlaylistNextMediaAutoplay();
    void playlistSizeResponse();
    void playFirstPlaylistItem();

    // panel - annot
    void displayAnnotationPanel();
    void deleteAnnot(Annotation& annotation);

    // panel - shot
    void extensionToolbarDisplayShotDetail();
    


    // position
    void simpleToolbarUpdateCursorPosition(int);
    void timelineSetPosition(int64_t);
    
    // adjustments
    void adjustmentChanged(const libvlc_video_adjust_option_t, const float);

    // formLine
    void formLineEditPlay();
    void formLineEditPause();
    
    // single record  
    void recordButtonUiUpdate();
    void recordButtonActivated();
    void recordButtonDectivated();

    // global record
    void globalRecordingStarted();
    void globalRecordingFinished();
    


};



#endif // SIGNALMANAGER_H
