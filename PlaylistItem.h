#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QWidget>
#include <Media.h>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <TimeFormatter.h>
#include <QImage>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

class PlaylistItem : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistItem(QWidget *parent = nullptr, const QString &mediaFilePath = "");
    void initStyle();
    void setThumbnail(QImage image);
    void updateTypeIcon();
    void updateThumbnail();
    MediaType getType() const { return m_mediaData->type(); }
    QSize thumbnailSize() const  {return m_thumbnailSize; }

private:
    Media *m_mediaData = nullptr;
    QLabel *m_mediaThumbnailLabel = nullptr;
    QPixmap *m_mediaThumbnailImage = nullptr;
    QPixmap *m_mediaTypeIcon = nullptr;
    QLabel *m_mediaDurationLabel = nullptr;
    QLabel *m_mediaTitleLabel = nullptr;
    QLabel *m_mediaTypeIconLabel = nullptr;
    unsigned int m_itemIndex = 0;
    QLabel *m_indexLabel = nullptr;
    QPushButton *m_deleteBtn = nullptr;
    bool m_isClicked = false;
    QSize m_thumbnailSize {64, 36};
    bool m_isCurrentMedia = false;
    QPoint m_dragStartPosition;

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setDurationLabel();
    void computeThumbnail();
    void setIndex(int);
    void playMedia();
    void setCurrentMedia(bool isCurrent);

signals:
    void deleteItemRequested(const unsigned int index);
    void playPlaylistItemRequested(const QString &filePath);
    void updatePlaylistCurrentIndex(unsigned int index);
    void updateImageRequested(int idShot, int64_t time, int64_t length, const QString& mediaPath, const QSize& targetSize);
};

#endif // PLAYLISTITEM_H
