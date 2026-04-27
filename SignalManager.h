#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "Shot.h"
#include <QObject>
#include <vlc/vlc.h>

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
    void mediaWidgetMediaFinished();
    void extensionToolbarDisplayShotDetail();
    void displayPlaylist();
    void playlistEjectPlayer();
    void simpleToolbarUpdateCursorPosition(int);
    void timelineSetPosition(int64_t);
    void playerWidgetSelectFileCanceled();
    void mediaVolumeChanged(const QString&);
    void mediaSpeedChanged(const QString&);
    void formLineEditPlay();
    void formLineEditPause();
    void newArrangementRequested(const PlayerLayoutArrangement&);
    void playerWidgetMediaDropped(const QStringList&);
    void windowMovedOrResized();
    void recordButtonUiUpdate();
    void addPlaylistItems(const QStringList&);
    void adjustmentChanged(const libvlc_video_adjust_option_t, const float);
};



#endif // SIGNALMANAGER_H
