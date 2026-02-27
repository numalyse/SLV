#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QWidget>
#include <TextManager.h>

/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QWidget
{
Q_OBJECT

public:
    explicit Toolbar(QWidget* parent = nullptr) : QWidget(parent) {
        m_playPauseBtn = new ToolbarToggleButton(
            this,
            true,
            "pause.png",
            TextManager::instance().get("tooltip_pause"),
            "play.png",
            TextManager::instance().get("tooltip_play")
        );
        m_stopBtn = new ToolbarButton(this, "stop.png", TextManager::instance().get("tooltip_stop"));
        m_ejectBtn = new ToolbarButton(this, "eject.png", TextManager::instance().get("tooltip_eject"));
        m_fullscreenBtn = new ToolbarToggleButton(this, false, "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"), "fullscreen.png", TextManager::instance().get("tooltip_fullscreen"));

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequested);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequested);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequested);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequested);
        qDebug() << "connect successful";
    }

    virtual ~Toolbar() = default;

    virtual void setFullscreenUI() = 0;
    virtual void setDefaultUI() = 0;

// Les classes filles pourront modifier ces widgets
protected: 

    ToolbarToggleButton* m_playPauseBtn = nullptr;
    ToolbarButton* m_stopBtn = nullptr;
    ToolbarButton* m_ejectBtn = nullptr;
    ToolbarToggleButton* m_fullscreenBtn = nullptr;

signals:

    void playRequested();
    void pauseRequested();
    void stopRequested();
    void ejectRequested();
    void enableFullscreenRequested();
    void disableFullscreenRequested();
};

#endif // TOOLBAR_H
