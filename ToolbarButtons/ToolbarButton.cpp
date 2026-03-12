#include "ToolbarButtons/ToolbarButton.h"


ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    if(iconName == ""){
        setText("Icon");
    }else {
        setIcon(QIcon(ICONS_PATH + iconName));
        setIconSize(QSize(12,30));
    }
    setToolTip(toolTipText);
}

