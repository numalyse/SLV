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
    QLabel* nameLabel() const { return m_nameLabel; }

    ToolbarToggleHoverButton* muteBtn() const { return static_cast<ToolbarToggleHoverButton*>(m_muteBtn); }
    ToolbarPopupButton* speedBtn() const { return m_speedBtn; }
    ToolbarToggleButton* loopBtn() const { return m_loopBtn; }
    ToolbarButton* removePlayerBtn() const { return m_removePlayerBtn; }
    
    void resetSlider();
    void stopSlider();

protected:
    QSlider* m_slider = nullptr;
    bool m_draggingSlider = false;
    float m_media_fps {};
    QTimer* m_seekTimer = nullptr;
    QLabel* m_currentTimeLabel = nullptr;
    QLabel* m_durationLabel = nullptr;
    QLabel* m_nameLabel = nullptr;
    bool m_discardVlcUiUpdates = false;

    QSlider* m_volumeSlider = nullptr;
    QLabel* m_volumeLabel = nullptr;
    ToolbarPopupButton* m_speedBtn = nullptr;
    QLabel* m_speedLabel = nullptr;
    ToolbarToggleButton* m_loopBtn = nullptr;
    ToolbarButton* m_removePlayerBtn = nullptr;
    ToolbarButton* m_duplicatePlayerBtn = nullptr;
    ToolbarPopupButton* m_langBtn = nullptr;

    // ToolbarButton* m_slowDownBtn
    // ToolbarButton* m_speedUpBtn;

public slots:
    void updateSliderRange(int64_t);
    void updateSliderValue(int64_t);
    void updateFps(double);

    void playUiUpdate();
    void pauseUiUpdate();
    void muteUiUpdate();
    void unmuteUiUpdate();
    void ejectUiUpdate();
    void stopUiUpdate();
    void enableLoopUiUpdate();
    void disableLoopUiUpdate();
    void nameUiUpdate(const QString &);
    void volumeUiUpdate(const QString &);
    void speedUiUpdate(const QString &);
    void disableLoopMode();

    void enableButtons();
    void disableButtons();

signals:
    void setPositionRequested(int64_t);
    void removePlayerRequest();
    void enableMuteRequest();
    void disableMuteRequest();
    void volumeChanged(int);
    void speedChanged(int);
    void enableLoopModeRequest();
    void disableLoopModeRequest();
    void duplicatePlayerRequested();
};

#endif
