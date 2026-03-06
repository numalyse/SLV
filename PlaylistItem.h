#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QWidget>
#include <Media.h>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <TimeFormatter.h>

class PlaylistItem : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistItem(QWidget *parent = nullptr, const QString &mediaFilePath = "");
    void initStyle();

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

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void setDurationLabel();
    void computeThumbnail();
    void setIndex(int);

signals:
    void deleteItemRequested(const unsigned int &index);
    void playPlaylistItemRequested(const QString &filePath);
};

#endif // PLAYLISTITEM_H
