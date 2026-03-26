#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QWidget>
#include <Media.h>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <TimeFormatter.h>
#include <QImage>

class PlaylistItem : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistItem(QWidget *parent = nullptr, const QString &mediaFilePath = "");
    void initStyle();
    void setThumbnail(QImage image);

private:
    Media *m_mediaData = nullptr;
    QLabel *m_mediaThumbnailLabel = nullptr;
    QPixmap *m_mediaThumbnailImage = nullptr;
    QLabel *m_mediaDurationLabel = nullptr;
    QLabel *m_mediaTitleLabel = nullptr;
    QLabel *m_mediaTypeIcon = nullptr; // mettre une pixmap dedans pour l'icône
    unsigned int m_itemIndex = 0;
    QLabel *m_indexLabel = nullptr;
    QPushButton *m_deleteBtn = nullptr;
    bool m_isClicked = false;
    QSize m_thumbnailSize {64, 36};

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void setDurationLabel();
    void computeThumbnail();
    void setIndex(int);
    void playMedia();

signals:
    void deleteItemRequested(const unsigned int &index);
    void playPlaylistItemRequested(const QString &filePath);
    void updatePlaylistCurrentIndex(unsigned int index);
    void updateImageRequested(int idShot, int64_t time, int64_t length, const QString& mediaPath, const QSize& targetSize);
};

#endif // PLAYLISTITEM_H
