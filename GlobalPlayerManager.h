#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include "Toolbars/Toolbar.h"
#include "PlayerLayoutManager.h"
#include "NavPanel.h"
#include "Media.h"
#include "Timeline/TimelineWidget.h"
#include "PlayerWidget.h"
#include "Timeline/ThumbnailWorker.h"
#include "SnapshotPopup.h"

#include <QWidget>


class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);

    Toolbar* toolbar() { return m_toolbarWidget; }
    
    void showAllToolbars(bool visible);
    bool isMouseOverAnyToolbar() const;
    bool isNavPanelOpen() { return m_navPanel->isOpen(); }

private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QWidget* m_playersWidget = nullptr;
    Toolbar* m_toolbarWidget = nullptr;
    NavPanel* m_navPanel = nullptr;
    TimelineWidget* m_timeline = nullptr;
    PlayerWidget* m_player = nullptr;
    QVBoxLayout* layout;
    QFrame* m_separationLine = nullptr;
    bool m_wasTimelineVisible = false;
    ThumbnailWorker* m_thumbnailWorker = nullptr;

    QPointer<SnapshotPopup> m_snapshotPopup = nullptr;

    void updateSplittersStyle(bool fullscreen);

public slots:


    void setPlayersFromPaths(QStringList);
    void enableFullscreenPlayer();
    void disableFullscreenPlayer();

    void enableSegmentation();
    void disableSegmentation();

    void updateContainer(PlayerWidget*, QWidget*, Toolbar*);
    void openNavPanel();
    void closeNavPanel();

    void resizeEvent(QResizeEvent *event);

    void setGlobalPlayState(bool);
    void setGlobalMuteState(bool);
    void setGlobalZoomState(bool);

    void toggleNavPanel(PanelType type);

private slots:
    void createTimelineWidget();
    void playPreviousMedia();
    void playNextMedia();

signals:
    void enableFullscreenMainRequested();
    void disableFullscreenMainRequested();
    void disableNavPanelRequested();
    void enableNavPanelRequested();
    void activePlayersMediaStateChanged(bool hasMedia);
    void activePlayersCountChanged(bool isSingle);
};

#endif // GLOBALPLAYERMANAGER_H
