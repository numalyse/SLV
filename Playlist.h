#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <PlaylistItem.h>
#include <QFileDialog>
#include <SignalManager.h>

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);

private:
    bool m_playlistLooping = false;
    bool m_randomized = false;
    unsigned int m_currentMediaIndex = 0;
    QVector<PlaylistItem*> m_items;
    QPushButton *m_addItemBtn = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_itemsLayout = nullptr;

signals:
    void openMediaFileRequested(const QString &filPath);
    void disableToolbarLoopRequested();

public slots:
    void addItemDialog();
    void addItemsFromPaths(const QStringList &filesPaths);
    void deleteItem(const unsigned int &index);
    void playPreviousMedia();
    void playNextMedia();
};

#endif // PLAYLIST_H
