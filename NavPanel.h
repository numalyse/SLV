#ifndef NAVPANEL_H
#define NAVPANEL_H

#include <QWidget>
#include <QBoxLayout>
#include <Playlist.h>

class NavPanel : public QWidget
{
    Q_OBJECT
public:
    explicit NavPanel(QWidget *parent = nullptr);
private:

    bool m_isOpen = false;
    QWidget *m_sideWidget = nullptr;
    QLayout *m_mainLayout = nullptr;
    Playlist *m_playlistWidget = nullptr;
    // ShotDetail *m_shotDetail = nullptr;

public slots:
    void showPanel();
    void hidePanel();
    // void displayPlaylist();
    // void displayShotDetail();
    void setPlaylistNewItem();
    void playPreviousMedia();
    void playNextMedia();

signals:
    void openMediaFileRequested(const QString &filePath);
    void disableToolbarLoopRequested();
};

#endif // NAVPANEL_H
