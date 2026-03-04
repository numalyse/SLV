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

    QSlider* slider() const { return m_slider; }
    double mediaFps() const { return m_media_fps; }
    QLabel* currentTimeLabel() const { return m_currentTimeLabel; }
    QLabel* durationLabel() const { return m_durationLabel; }

    ToolbarToggleHoverButton* muteBtn() const { return m_muteBtn; }
    ToolbarPopupButton* speedBtn() const { return m_speedBtn; }
    ToolbarToggleButton* loopBtn() const { return m_loopBtn; }
    ToolbarButton* removePlayerBtn() const { return m_removePlayerBtn; }


protected:
    QSlider* m_slider = nullptr;
    bool m_draggingSlider = false;
    float m_media_fps {};
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
    void enableMuteRequest();
    void disableMuteRequest();
    void volumeChanged(int);
    void speedChanged(int);
    void enableLoopModeRequest();
    void disableLoopModeRequest();
};

#endif
