#ifndef EXTENSIONTOOLBAR_H
#define EXTENSIONTOOLBAR_H 

#include "Toolbars/Toolbar.h"
#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QWidget>

/// @brief Extension de la Toolbar avancée.
class ExtensionToolbar : public QWidget
{
Q_OBJECT

public:
    explicit ExtensionToolbar(QWidget* parent = nullptr);

    void setFullscreenUI();
    void setDefaultUI();
    
    ToolbarToggleButton* m_zoomBtn = nullptr;
    ToolbarToggleButton* m_hideImgBtn= nullptr;
    ToolbarButton* m_prevFrameBtn= nullptr;
    ToolbarButton* m_nextFrameBtn= nullptr;
    ToolbarButton* m_backwardBtn= nullptr;
    ToolbarButton* m_forwardBtn= nullptr;
    ToolbarButton* m_rotateBtn= nullptr;
    ToolbarToggleButton* m_recordBtn = nullptr;
    ToolbarToggleButton* m_segmBtn= nullptr;
    ToolbarButton* m_compoRuleBtn= nullptr;
    ToolbarPopupButton* m_invBtn = nullptr;
    ToolbarButton* m_verticalInvBtn= nullptr;
    ToolbarButton* m_horizontalInvBtn= nullptr;
    // ToolbarButton* m_abloopBtn = nullptr;
    // ToolbarPopupButton* m_multiviewBtn;


    // playlist btn ?
    // A / B Button voir comment

public slots:
    void enableButtons();
    void disableButtons();
    void updateRecordButtonUI();

signals:
    void enableRecordRequested();
    void disableRecordRequested();
    void enableSegmentationRequested();
    void disableSegmentationRequested();
    void moveTimeBackwardRequested();
    void moveTimeForwardRequested();
    void rotateRequested();
};

#endif
