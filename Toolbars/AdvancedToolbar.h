#ifndef ADVANCEDTOOLBAR_H
#define ADVANCEDTOOLBAR_H 

#include "Toolbars/SimpleToolbar.h"
#include "Toolbars/ExtensionToolbar.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h"
#include "ToolbarButtons/ToolbarToggleHoverButton.h"

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

    
private:
    ToolbarButton* m_prevMediaBtn = nullptr;
    ToolbarButton* m_nextMediaBtn = nullptr;

    ToolbarToggleButton* m_extensionBtn = nullptr;
    ExtensionToolbar* m_extensionToolbar = nullptr;

signals:
    void enableSegmentationRequest();
    void disableSegmentationRequest();
    void previousMediaRequested();
    void nextMediaRequested();


};

#endif
