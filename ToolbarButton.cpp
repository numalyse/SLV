#include "ToolbarButton.h"

/// @brief Créer un bouton avec une icone et un tooltip
/// @param parent QWidget*
/// @param iconName const QString&, nom de l'icone ex : "pause.png"
/// @param toolTipText const QString &
ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    if(iconName == ""){
        setText("Icon");
    }else {
        setIcon(QIcon(ICONS_PATH + iconName));
        setIconSize(QSize(10,30));
    }
    setToolTip(toolTipText);
}

