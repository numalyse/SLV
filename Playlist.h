#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <PlaylistItem.h>
#include <QFileDialog>
#include <SignalManager.h>
#include <QImage>
#include "./ToolbarButtons/ToolbarToggleButton.h"

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);
    void updateThumbnail(int playlistItemId, QImage image);

    void resizeEvent(QResizeEvent *event);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    bool m_playlistLooping = false;
    bool m_randomized = false;
    unsigned int m_currentMediaIndex = 0;
    QVector<PlaylistItem*> m_items;
    ToolbarToggleButton *m_loopItemBtn = nullptr;
    ToolbarToggleButton *m_shuffleItemBtn = nullptr;
    QPushButton *m_addItemBtn = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_itemsLayout = nullptr;

signals:
    void openMediaFileRequested(const QString &filePath);
    void disableToolbarLoopRequested();
    void updateImageRequested(int idShot, int64_t time, int64_t length, const QString& mediaPath, const QSize& targetSize);

public slots:
    void addItemDialog();
    void addItemsFromPaths(const QStringList &filesPaths);
    void deleteItem(const unsigned int index);
    void playMedia(const QString &filePath);
    void playPreviousMedia();
    void playNextMedia();
    void enableLoop();
    void disableLoop();
    void enableShuffle();
    void disableShuffle();

private slots:
    void updateItemIndices();
    void updateLayout();
};

#endif // PLAYLIST_H
