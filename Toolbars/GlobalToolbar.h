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
    void disableFullscreenRequested() override;

public slots:
    void enableButtons();
    void disableButtons();
    void enableFullscreenUiUpdate();
    void disableFullscreenUiUpdate();

signals:
    void enableMute();
    void disableMute();
private: 
    void addShortcuts();

    QVector<QShortcut*> m_globalShortcuts;
};

#endif
