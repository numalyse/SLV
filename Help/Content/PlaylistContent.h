#ifndef PLAYLISTCONTENT_H
#define PLAYLISTCONTENT_H

#include "../Base/CategoryBase.h"

class PlaylistContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit PlaylistContent(QWidget* parent = nullptr);

private:
    QWidget *presentation(const QString &subcategoryName);
    QWidget *managePlaylist(const QString &subcategoryName);
    // QWidget* introduction(const QString& subcategoryName);
};

#endif