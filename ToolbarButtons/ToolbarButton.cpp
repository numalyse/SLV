#include "ToolbarButtons/ToolbarButton.h"


ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    if(iconName == ""){
        setText("Icon");
    }else {
        setIcon(QIcon(ICONS_PATH + iconName));
        // setIconSize(QSize(12,30));
        setFixedSize(30, 30);
        // setStyleSheet("background-color: palette(button);");
    }
    setToolTip(toolTipText);
}

