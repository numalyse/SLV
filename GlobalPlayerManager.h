#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include "Toolbars/Toolbar.h"
#include "PlayerLayoutManager.h"
#include "Media.h"
#include "Project.h"

#include <QWidget>


class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);
    QWidget* m_playersWidget = nullptr;
    Toolbar* m_toolbarWidget = nullptr;
    Project* m_project = nullptr;
private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QVBoxLayout* layout;

public slots:


    void setPlayersFromPaths(QStringList);
    void enableFullscreenPlayer();
    void disableFullscreenPlayer();

    void updateContainer(Media*, QWidget*, Toolbar*);

    void setGlobalPlayState(bool);
    void setGlobalMuteState(bool);

signals:
    void enableFullscreenMainRequested();
    void disableFullscreenMainRequested();
};

#endif // GLOBALPLAYERMANAGER_H
