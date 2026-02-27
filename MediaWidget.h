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
    void setActive(bool active);
    void setMediaFromPath(const QString& filePath);

public slots:
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void eject();

private:
    libvlc_instance_t *m_vlc;
    libvlc_media_player_t *m_player;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void activated(MediaWidget* self);
};

#endif // MEDIAWIDGET_H
