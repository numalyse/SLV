#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include "PlaylistItem.h"
#include <QFileDialog>
#include "SignalManager.h"
#include <QImage>
#include "./ToolbarButtons/ToolbarToggleButton.h"
#include "./ToolbarButtons/ToolbarPopupButton.h"
#include <random>
#include <QRadioButton>
#include <QButtonGroup>

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);
    void updateThumbnail(int playlistItemId, QImage image);
    QStringList dataHasValidUrls(const QMimeData *mimedata) const;

    void resizeEvent(QResizeEvent *event) override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    bool m_playlistLooping = false;
    bool m_playlistShuffled = false;
    unsigned int m_currentMediaIndex = 0;
    QVector<unsigned int> m_itemsShuffleOrder;
    QVector<unsigned int> m_itemsSortOrder;
    QVector<PlaylistItem*> m_items;
    ToolbarToggleButton *m_loopItemBtn = nullptr;
    ToolbarToggleButton *m_shuffleItemBtn = nullptr;
    ToolbarPopupButton *m_sortPlaylistBtn = nullptr;
    QPushButton *m_addItemBtn = nullptr;
    QPushButton *m_deleteAllBtn = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_itemsLayout = nullptr;
    QButtonGroup *m_sortButtons;
    void createSortBtn();

signals:
    void openMediaFileRequested(const QString &filePath);
    void disableToolbarLoopRequested();

public slots:
    void addItemDialog();
    void deleteAllItemsDialog();
    void addItemsFromPaths(const QStringList &filesPaths);
    void deleteItem(const unsigned int index);
    void deleteAllItems();
    void playMedia(const QString &filePath, const bool isClicked);
    void playPreviousMedia();
    void playNextMedia();
    void enableLoop();
    void disableLoop();
    void enableShuffle();
    void disableShuffle();
    void sortPlaylist(int id, bool checked);

private slots:
    void updateItemIndices();
    void updateLayout();
};

#endif // PLAYLIST_H
