#ifndef GLOBALTOOLBAR_H
#define GLOBALTOOLBAR_H 

#include "Toolbars/Toolbar.h"

#include <QWidget>

/// @brief Toolbar globale, utilisé pour les effectuer des actions sur tous les lecteurs en mode synchronisé.
class GlobalToolbar : public Toolbar
{
Q_OBJECT

public:
    explicit GlobalToolbar(QWidget* parent = nullptr);

    void setFullscreenUI() override;
    void setDefaultUI() override;

signals:
    void enableMute();
    void disableMute();


};

#endif
