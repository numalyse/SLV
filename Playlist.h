#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <PlaylistItem.h>
#include <QFileDialog>

class Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Playlist(QWidget *parent = nullptr);

private:
    bool m_looping = false;
    bool m_randomized = false;
    QVector<PlaylistItem*> m_items;
    QPushButton *m_addItemBtn = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_itemsLayout = nullptr;

signals:
    void openMediaFileRequested(const QString &filPath);

public slots:
    void addItemDialog();
    void addItemsFromPaths(const QStringList &filesPaths);
    void deleteItem(const unsigned int &index);
};

#endif // PLAYLIST_H
