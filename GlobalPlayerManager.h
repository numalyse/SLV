#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include "Toolbars/Toolbar.h"
#include "PlayerLayoutManager.h"
#include "NavPanel.h"
#include "Media.h"
#include "TimelineWidget.h"
#include "PlayerWidget.h"

#include <QWidget>


class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);

private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QWidget* m_playersWidget = nullptr;
    Toolbar* m_toolbarWidget = nullptr;
    NavPanel* m_navPanel = nullptr;
    TimelineWidget* m_timeline = nullptr;
    PlayerWidget* m_player = nullptr;
    QVBoxLayout* layout;

public slots:


    void setPlayersFromPaths(QStringList);
    void enableFullscreenPlayer();
    void disableFullscreenPlayer();

    void enableSegmentation();
    void disableSegmentation();

    void updateContainer(PlayerWidget*, Media*, QWidget*, Toolbar*);
    void openNavPanel();
    void closeNavPanel();

    void setGlobalPlayState(bool);
    void setGlobalMuteState(bool);

private slots:
    void createTimelineWidget();
    void playPreviousMedia();
    void playNextMedia();

signals:
    void enableFullscreenMainRequested();
    void disableFullscreenMainRequested();
    void disableNavPanelRequested();
    void enableNavPanelRequested();
};

#endif // GLOBALPLAYERMANAGER_H
