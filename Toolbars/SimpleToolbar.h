#ifndef SIMPLETOOLBAR_H
#define SIMPLETOOLBAR_H 

#include "Toolbars/Toolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

/// @brief Toolbar simple, utilisé pour les lecteurs en mode synchronisé.
class SimpleToolbar : public Toolbar
{
Q_OBJECT

public:
    explicit SimpleToolbar(QWidget* parent = nullptr);

    void setFullscreenUI() override;
    void setDefaultUI() override;

protected:
    
    QLabel* m_currentTimeLabel = nullptr;
    QLabel* m_durationLabel = nullptr;

    QSlider* m_slider = nullptr;

    ToolbarToggleHoverButton* m_muteBtn = nullptr;
    ToolbarPopupButton* m_speedBtn = nullptr;
    ToolbarToggleButton* m_loopBtn = nullptr;
    ToolbarButton* m_removePlayerBtn = nullptr;

    // ToolbarButton* m_slowDownBtn
    // ToolbarButton* m_speedUpBtn;

signals:
    void removePlayerRequest();
    void enableMuteRequest();
    void disableMuteRequest();
    void volumeChanged(int);
};

#endif
