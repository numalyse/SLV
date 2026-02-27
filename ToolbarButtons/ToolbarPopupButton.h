#ifndef TOOLBARPOPUPBUTTON_H
#define TOOLBARPOPUPBUTTON_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QPushButton>
#include <QTimer>

class ToolbarPopupButton: public ToolbarButton
{
Q_OBJECT

public:

    /// @brief Créer un bouton qui affiche une popup lors du clique. Si le bouton de la souris est activé pendant que la popup est affichée, la popup est cachée.
    /// @param widgetToDisplay QWidget* à afficher dans la popup lors du clique, Ce QWidget sera détruit par ce bouton
    explicit ToolbarPopupButton(
        QWidget* parent = nullptr,
        QWidget* widgetToDisplay = nullptr,
        const QString& iconName = "", 
        const QString& toolTipText = "ToolTipOn"
    );

    ~ToolbarPopupButton() { delete m_widgetToDisplay; }; // Le widget n'a pas de parent, il doit être détruit manuellement

private:
    QWidget* m_widgetToDisplay = nullptr;

    // Flag pour empêcher que la popup ne se cache puis se réaffiche instantanément quand on clique sur le bouton alors qu'elle est déjà ouverte.
    bool m_blockNextShow = false; 

    /// @brief Déplace le widget popup centré au dessus du bouton, offset sur la hauteur fixé a 10 px
    void moveWidgetOnTop();

private slots:
    /// @brief Slot privé qui affiche la popup au dessus du bouton
    /// (si la popup est déjà ouverte cela aura pour effet de cacher la popup)
    void displayPopup();

protected:

    /// @brief Detecte si l'on à recliqué sur le bouton pendant que la popup était ouverte. Si c'est le cas m_blockNextShow = true
    /// @param watched QWidget* de la popup
    /// @param event QEvent::Hide
    /// @return 
    bool eventFilter(QObject *watched, QEvent *event) override;


};

#endif