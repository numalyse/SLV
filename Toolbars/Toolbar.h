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

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, [&](){
            emit playRequested();
        });
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, [&](){emit pauseRequested();});
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequested);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequested);
        connect(m_fullscreenBtn, &ToolbarButton::clicked, this, &Toolbar::fullscreenRequested);
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
    ToolbarButton* m_fullscreenBtn = nullptr;

signals:
    void playRequested();
    void pauseRequested();
    void stopRequested();
    void ejectRequested();
    void fullscreenRequested();
};

#endif // TOOLBAR_H
