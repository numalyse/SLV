#ifndef EXTENSIONTOOLBAR_H
#define EXTENSIONTOOLBAR_H 

#include "Toolbars/Toolbar.h"
#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"
#include "OverlayMode.h"
#include "AdjustmentsWidget.h"

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QVector>
#include <vlc/vlc.h>

/// @brief Extension de la Toolbar avancée.
class ExtensionToolbar : public QWidget
{
Q_OBJECT

public:
    explicit ExtensionToolbar(QWidget* parent = nullptr);

    /// @brief destructor that deletes the global shortcuts since they are declarer as child of main window
    ~ExtensionToolbar();

    void setOverlayMode(int index);
    void setFullscreenUI();
    void setDefaultUI();
    void initAdjustmentLayout();
    
    //QComboBox* comboBoxCompoRuleBtn = nullptr;

    ToolbarToggleButton* m_zoomBtn = nullptr;
    ToolbarToggleButton* m_hideImgBtn= nullptr;
    ToolbarButton* m_prevFrameBtn= nullptr;
    ToolbarButton* m_nextFrameBtn= nullptr;
    ToolbarButton* m_backwardBtn= nullptr;
    ToolbarButton* m_forwardBtn= nullptr;
    ToolbarButton* m_rotateBtn= nullptr;
    ToolbarToggleButton* m_recordBtn = nullptr;
    ToolbarToggleButton* m_segmBtn= nullptr;
    ToolbarPopupButton* m_compoRuleBtn = nullptr;
    ToolbarPopupButton* m_invBtn = nullptr;
    ToolbarToggleButton* m_verticalInvBtn= nullptr;
    ToolbarToggleButton* m_horizontalInvBtn= nullptr;
    ToolbarToggleButton* m_drawingBtn = nullptr;
    ToolbarPopupButton* m_adjustmentsBtn = nullptr;
    AdjustmentsWidget* m_adjustmentWidget = nullptr;

    // playlist btn ?
    // A / B Button voir comment

public slots:
    void enableButtons();
    void disableButtons();
    void updateRecordButtonUI();
    void updateHFlipButtonUI();
    void updateVFlipButtonUI();
    void updateDrawingButtonUI();

private slots:
    void updateDrawingMode();
    void updateOverlayMode(); 


private:
    /// @brief Adds global shortcuts (that are child of the mainWindow) to be usable when the widget is hidden
    void addShortcuts();

    /// @brief Delete via delete later all of the global shortcuts
    void clearShortcuts();

    QComboBox* m_compoRuleComboBox;
    QCheckBox* m_compoRuleCheckboxVFlip;
    QCheckBox* m_compoRuleCheckboxHFlip;

    QVector<QShortcut*> m_globalShortcuts;

signals:
    void enableRecordRequested();
    void disableRecordRequested();
    void enableSegmentationRequested();
    void disableSegmentationRequested();
    void moveTimeBackwardRequested();
    void moveTimeForwardRequested();
    void rotateRequested();
    void horizontalFlipRequested();
    void verticalFlipRequested();
    void setOverlayModeRequested(OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked);
    void showDrawingModeRequested(bool isEnabled);
    void prevFrameRequested();
    void nextFrameRequested();
    void adjustmentChangeRequested(const libvlc_video_adjust_option_t, const float);
    void resetAdjustmentsRequested();

};

#endif
