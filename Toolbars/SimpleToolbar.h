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
    QSlider* m_slider = nullptr;
    bool m_draggingSlider = false;
    float m_media_fps = 0.0;
    QTimer* m_seekTimer = nullptr;
    QLabel* m_currentTimeLabel = nullptr;
    QLabel* m_durationLabel = nullptr;

    ToolbarToggleHoverButton* m_muteBtn = nullptr;
    ToolbarPopupButton* m_speedBtn = nullptr;
    ToolbarToggleButton* m_loopBtn = nullptr;
    ToolbarButton* m_removePlayerBtn = nullptr;

    // ToolbarButton* m_slowDownBtn
    // ToolbarButton* m_speedUpBtn;

public slots:
    void updateSliderRange(int64_t);
    void updateSliderValue(int64_t);
    void updateFps(double);

signals:
    void setPositionRequested(int64_t);
    void removePlayerRequest();
};

#endif
