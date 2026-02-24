#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButton.h"
#include "ToolbarToggleButton.h"

#include <QToolBar>

/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QToolBar
{
Q_OBJECT

public:
    explicit Toolbar(QWidget* parent = nullptr) : QToolBar(parent) {
    }

    virtual ~Toolbar() = default;

    virtual void setFullscreenUI() = 0;
    virtual void setDefaultUI() = 0;

// Les classes filles pourront modifier ces widgets
protected: 

    // Widget qui contient les layouts / boutons 
    QWidget* m_containerWidget = nullptr; 

    ToolbarToggleButton* m_playPauseBtn = nullptr;
    ToolbarButton* m_stopBtn = nullptr;
    ToolbarButton* m_ejectBtn = nullptr;
    ToolbarButton* m_fullscreenBtn = nullptr;
    
};

#endif // TOOLBAR_H
