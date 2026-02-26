#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "MediaWidget.h"
#include "Toolbars/SimpleToolbar.h"
#include "Toolbars/Toolbar.h"
#include "Media.h"

#include <QWidget>
#include <qmainwindow.h>
#include <vlc/vlc.h>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    // ~PlayerWidget();

    // Contrôle du média
    void play();
    void pause();
    void stop();
    void togglePlayPause();

    void setActive(bool active);
    void removeMedia();
    void setMediaFromPath(const QString& filePath);

signals:
    void addPlayer();
    void removePlayer(PlayerWidget* self);

private:
    MediaWidget *m_mediaWidget;

    Media *m_media = nullptr;
    Toolbar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QAction* m_addPlayerAction;
    QAction* m_removePlayerAction;
    MediaWidget* m_videoWidget = nullptr;

};

#endif // PLAYERWIDGET_H
