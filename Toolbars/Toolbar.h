#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QWidget>

/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QWidget
{
Q_OBJECT

public:
    explicit Toolbar(QWidget* parent = nullptr) : QWidget(parent) {
    }

    virtual ~Toolbar() = default;

    virtual void setFullscreenUI() = 0;
    virtual void setDefaultUI() = 0;

// Les classes filles pourront modifier ces widgets
protected: 

    ToolbarToggleButton* m_playPauseBtn = nullptr;
    ToolbarButton* m_stopBtn = nullptr;
    ToolbarButton* m_ejectBtn = nullptr;
    ToolbarButton* m_fullscreenBtn = nullptr;
};

#endif // TOOLBAR_H
