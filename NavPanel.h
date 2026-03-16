#ifndef NAVPANEL_H
#define NAVPANEL_H

#include "Shot.h"
#include "ShotDetail.h"

#include <QWidget>
#include <QBoxLayout>
#include <Playlist.h>
#include <QStackedWidget>

class NavPanel : public QWidget
{
    Q_OBJECT
public:
    explicit NavPanel(QWidget *parent = nullptr);
private:

    bool m_isOpen = false;
    QStackedWidget *m_sideWidget = nullptr;
    QLayout *m_mainLayout = nullptr;
    Playlist *m_playlistWidget = nullptr;
    ShotDetail *m_shotDetail = nullptr;

public slots:
    void showPanel();
    void hidePanel();
    // void displayPlaylist();
    // void displayShotDetail();
    void setPlaylistNewItem();
    void playPreviousMedia();
    void playNextMedia();
    void displayShotDetail();
    void displayPlaylist();
    void timelineWidgetUpdateShotDetail(int, int, Shot*);

signals:
    void openMediaFileRequested(const QString &filePath);
    void disableToolbarLoopRequested();
    void goToShotRequest(int);

};

#endif // NAVPANEL_H
