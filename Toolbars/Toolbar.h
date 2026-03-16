#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QWidget>
#include <TextManager.h>
#include <SignalManager.h>

/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QWidget
{
Q_OBJECT

public:

    explicit Toolbar(QWidget* parent = nullptr) : QWidget(parent) {
        m_playPauseBtn = new ToolbarToggleButton(
            this,
            true,
            "pause_white",
            TextManager::instance().get("tooltip_pause"),
            "play_white",
            TextManager::instance().get("tooltip_play")
        );
        m_playPauseBtn->setButtonState(false);
        m_playPauseBtn->setEnabled(true);
        
        m_stopBtn = new ToolbarButton(this, "stop_white", TextManager::instance().get("tooltip_stop"));
        m_ejectBtn = new ToolbarButton(this, "eject_white", TextManager::instance().get("tooltip_eject"));
        m_fullscreenBtn = new ToolbarToggleButton(this, false, "fullscreen_white", TextManager::instance().get("tooltip_fullscreen"), "fullscreen_white", TextManager::instance().get("tooltip_fullscreen"));
        m_screenshotBtn = new ToolbarButton(this, "capture_white", TextManager::instance().get("tooltip_capture"));

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequest);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequest);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequest);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequest);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequest);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequest);
        connect(m_screenshotBtn, &ToolbarButton::clicked, this, &Toolbar::screenshotRequest);
        connect(&SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled, m_playPauseBtn, [this](){ m_playPauseBtn->setButtonState(false); });
        connect(this, &Toolbar::selectFilePlayCanceled, &SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled);
    }

    ToolbarToggleButton* playPauseBtn() const { return m_playPauseBtn; }
    ToolbarButton* stopBtn() const { return m_stopBtn; }
    ToolbarButton* ejectBtn() const { return m_ejectBtn; }
    ToolbarToggleButton* fullscreenBtn() const { return m_fullscreenBtn; }
    ToolbarToggleButton* muteBtn() { return m_muteBtn; };

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
    ToolbarButton* m_screenshotBtn = nullptr;
    ToolbarToggleButton* m_muteBtn = nullptr;

signals:
    void playRequest();
    void pauseRequest();
    void stopRequest();
    void ejectRequest();
    void enableFullscreenRequest();
    void disableFullscreenRequest();
    void enableMuteRequest();
    void screenshotRequest();
    void selectFilePlayCanceled();
};

#endif // TOOLBAR_H
