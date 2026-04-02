#ifndef SIMPLETOOLBAR_H
#define SIMPLETOOLBAR_H 

#include "Toolbars/Toolbar.h"
#include "Toolbars/TimeEdit.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QComboBox>

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
    TimeEdit* currentTimeLE() const { return m_timeEdit; }
    QPushButton* durationBtn() const { return m_durationBtn; }
    bool showRemainingTime() const { return m_showRemainingTime; }
    QLabel* nameLabel() const { return m_nameLabel; }

    ToolbarToggleHoverButton* muteBtn() const { return static_cast<ToolbarToggleHoverButton*>(m_muteBtn); }
    ToolbarToggleHoverButton* speedBtn() const { return m_speedBtn; }
    ToolbarToggleButton* loopBtn() const { return m_loopBtn; }
    ToolbarButton* removePlayerBtn() const { return m_removePlayerBtn; }
    
    void resetSlider();
    void stopSlider();

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
    void enableFullscreenUiUpdate();
    void disableFullscreenUiUpdate();
    void disableLoopMode();

    void enableButtons();
    void disableButtons();

    void updateAudioTracks(const QList<QPair<int, QString>>& tracks);
    void updateSubtitlesTracks(const QList<QPair<int, QString>>& tracks);

    void setAudioTrackDefault();
    void setSubtitlesTrackDefault();
    void setAudioTrack(int index);
    //void setAudioTrack(int index, bool emitSimpleToolbarRequest);
    void setSubtitlesTrack(int index);



protected:
    void createSlider();
    void createTimeTotBtn();
    void createTimeEdit();

    void updateDurationText();

    QSlider* m_slider = nullptr;
    bool m_draggingSlider = false;
    double m_media_fps {};
    int64_t m_media_duration {};
    QTimer* m_seekTimer = nullptr;
    int m_seekPendingTime = 50;
    TimeEdit* m_timeEdit = nullptr;
    bool m_editingTime = false;
    QPushButton* m_durationBtn; 
    bool m_showRemainingTime = false;
    QLabel* m_nameLabel = nullptr;
    bool m_discardVlcUiUpdates = false;

    QSlider* m_volumeSlider = nullptr;
    QLabel* m_volumeLabel = nullptr;
    ToolbarToggleHoverButton* m_speedBtn = nullptr;
    QLabel* m_speedLabel = nullptr;
    ToolbarToggleButton* m_loopBtn = nullptr;
    ToolbarButton* m_removePlayerBtn = nullptr;
    ToolbarButton* m_duplicatePlayerBtn = nullptr;

    QComboBox* m_audioLangComboBox = nullptr;
    QComboBox* m_subLangComboBox = nullptr;
    ToolbarPopupButton* m_langBtn = nullptr;


protected slots: 
    virtual void onSliderPressed();
    virtual void onSliderReleased();
    virtual void onSliderMoved(int value);
    virtual void duplicatePlayerAction();

signals:
    void setPositionRequested(int64_t);
    void removePlayerRequest();
    void setAudioTrackRequested(int);
    void setSubtitlesTrackRequested(int);
    void enableMuteRequest();
    void disableMuteRequest();
    void volumeChanged(int);
    void speedChanged(int);
    void enableLoopModeRequest();
    void disableLoopModeRequest();
    void duplicatePlayerRequested();
    void setCursorPositionRequested(int64_t);
    
};

#endif
