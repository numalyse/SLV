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

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequest);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequest);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequest);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequest);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequest);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequest);
        qDebug() << "connect successful";
    }

    virtual ~Toolbar() = default;

    /// @brief Met à jour le layout pour afficher l'interface en plein écran
    virtual void setFullscreenUI() = 0;

    /// @brief Met à jour le layout pour afficher l'interface par défaut
    virtual void setDefaultUI() = 0;

// Les classes filles pourront modifier ces widgets
protected: 
    ToolbarToggleButton* m_playPauseBtn = nullptr;
    ToolbarButton* m_stopBtn = nullptr;
    ToolbarButton* m_ejectBtn = nullptr;
    ToolbarToggleButton* m_fullscreenBtn = nullptr;

signals:
    void playRequest();
    void pauseRequest();
    void stopRequest();
    void ejectRequest();
    void enableFullscreenRequest();
    void disableFullscreenRequest();
    void enableMuteRequest();
};

#endif // TOOLBAR_H
