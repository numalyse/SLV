#ifndef ADVANCEDTOOLBAR_H
#define ADVANCEDTOOLBAR_H 

#include "Toolbars/SimpleToolbar.h"
#include "Toolbars/ExtensionToolbar.h"
#include "Toolbars/TimeEdit.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include "OverlayMode.h"

#include <QWidget>
#include <QLabel>
#include <QSlider>


/// @brief Toolbar Avancé, utilisé pour la lecture classique, peut être étendu pour afficher des contrôles supplémentaires.
class AdvancedToolbar : public SimpleToolbar
{
Q_OBJECT

public:
    explicit AdvancedToolbar(QWidget* parent = nullptr);
    explicit AdvancedToolbar(QWidget* parent, SimpleToolbar* toolbar);

    void setFullscreenUI() override;
    void setDefaultUI() override;
    void setExtractable(const bool extractable) { m_extractable = extractable; m_extractSequenceBtn->setEnabled(true); };

public slots:
    void enableButtons();
    void disableButtons();
    void enableSlider();
    void disableSlider();

    virtual void ejectRequested() override;
    
private:
    ToolbarButton* m_prevMediaBtn = nullptr;
    ToolbarButton* m_nextMediaBtn = nullptr;

    ToolbarToggleButton* m_extensionBtn = nullptr;
    ExtensionToolbar* m_extensionToolbar = nullptr;

protected slots: 
    virtual void onSliderPressed() override;
    virtual void onSliderReleased() override;
    virtual void onSliderMoved(int value) override;
    virtual void duplicatePlayerAction() override;

    
signals:
    void enableRecordRequested();
    void disableRecordRequested();
    void enableSegmentationRequest();
    void disableSegmentationRequest();
    void moveTimeBackwardRequested();
    void moveTimeForwardRequested();
    void rotateRequested();
    void hFlipUiUpdate();
    void vFlipUiUpdate();
    void hFlipRequested();
    void vFlipRequested();
    void prevFrameRequested();
    void nextFrameRequested();
    void previousMediaRequested();
    void nextMediaRequested();
    void toolbarCursorPositionRequested(int);
    void setOverlayModeRequested(OverlayMode overlayMode, bool vFlipChecked, bool hFlipChecked);

};

#endif
