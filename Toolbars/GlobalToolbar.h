#ifndef GLOBALTOOLBAR_H
#define GLOBALTOOLBAR_H 

#include "Toolbars/Toolbar.h"

#include <QWidget>
#include <QVector>
#include <QShortcut>

/// @brief Toolbar globale, utilisé pour les effectuer des actions sur tous les lecteurs en mode synchronisé.
class GlobalToolbar : public Toolbar
{
Q_OBJECT

public:
    explicit GlobalToolbar(QWidget* parent = nullptr);

    ~GlobalToolbar();

    void setDefaultUI() override;
    void setFullscreenUI(int bottomMargin = GlobalToolbar::s_bottomMarginFullscreen) override;
    void disableFullscreenRequested() override;

    void updateFullscreenPosition() override;

public slots:
    void enableButtons();
    void disableButtons();
    void enableFullscreenUiUpdate();
    void disableFullscreenUiUpdate();

signals:
    void enableMute();
    void disableMute();
    void enableRecord();
    void disableRecord();
private: 

    QVector<QShortcut*> m_globalShortcuts;
    static constexpr int s_bottomMarginFullscreen = 10;
    int fullscreenBottomMargin() const override { return s_bottomMarginFullscreen; }
    QSpacerItem* m_extraSpacingItem = nullptr;
    ToolbarToggleButton* m_multiviewRecordBtn = nullptr;

    void addShortcuts();
};

#endif
