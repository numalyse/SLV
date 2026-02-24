#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <qmainwindow.h>
#include <vlc/vlc.h>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    ~PlayerWidget();

    // Contrôle du média
    void play();
    void pause();
    void stop();
    void togglePlayPause();

    void setActive(bool active);

signals:
    void activated(PlayerWidget* self);
    void addPlayer();
    void removePlayer(PlayerWidget* self);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    libvlc_instance_t *m_vlc;
    libvlc_media_player_t *m_player;

    QToolBar* m_toolBar = nullptr;
    QAction* m_actionPlayPause;
    QAction* m_actionStop;
    QWidget* m_videoWidget = nullptr;

};

#endif // PLAYERWIDGET_H
