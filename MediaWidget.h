#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <vlc/vlc.h>
#include <QWidget>

class MediaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MediaWidget(QWidget *parent = nullptr);
    ~MediaWidget();
    void managePlayerSystem();
    void setActive(bool active);
    void setMediaFromPath(const QString& filePath);

public slots:
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void eject();
    void mute();
    void unmute();
    void setVolume(int vol);

private:
    libvlc_instance_t *m_vlc = nullptr;
    libvlc_media_player_t *m_player = nullptr;
    int lastVolume = 100;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void activated(MediaWidget* self);
};

#endif // MEDIAWIDGET_H
