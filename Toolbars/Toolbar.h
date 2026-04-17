#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QWidget>
#include "PrefManager.h"
#include "SignalManager.h"

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
            PrefManager::instance().getText("tooltip_pause"),
            "play_white",
            PrefManager::instance().getText("tooltip_play")
        );
        m_playPauseBtn->setButtonState(false);
        m_playPauseBtn->setEnabled(true);
        
        m_stopBtn = new ToolbarButton(this, "stop_white", PrefManager::instance().getText("tooltip_stop"));
        m_ejectBtn = new ToolbarButton(this, "eject_white", PrefManager::instance().getText("tooltip_eject"));
        m_fullscreenBtn = new ToolbarToggleButton(this, false, "fullscreen_off_white", PrefManager::instance().getText("tooltip_fullscreen"), "fullscreen_white", PrefManager::instance().getText("tooltip_fullscreen"));
        m_screenshotBtn = new ToolbarButton(this, "capture_white", PrefManager::instance().getText("tooltip_capture"));

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequest);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequest);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequest);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequested);
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

public slots:
    virtual void ejectRequested(){
        emit ejectRequest();
    };

    void enableFullscreenRequested(){
        // permet d'ajouter un shortcut a la toolbar pour pouvoir quitter le fullscreen avec le raccourcis meme si la toolbar n'a pas de raccourcis
        m_fullscreenBtn->setShortcut(QKeySequence(PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "exit_fullscreen")));
        emit enableFullscreenRequest();
    }

    virtual void disableFullscreenRequested(){
        // par défaut on supprime le shortcut, dans advanced / global toolbar on override cette fonction pour le rajouter le bon
        m_fullscreenBtn->setShortcut(QKeySequence()); 
        emit disableFullscreenRequest();
    }


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
