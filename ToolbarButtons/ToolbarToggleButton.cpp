#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarToggleButton.h"



ToolbarToggleButton::ToolbarToggleButton(
    QWidget *parent, 
    bool state,
    const QString &iconNameOn, 
    const QString &toolTipTextOn, 
    const QString &iconNameOff, 
    const QString &toolTipTextOff)  : QPushButton(parent)
{

    m_iconPathOn = ICONS_PATH + iconNameOn;
    m_toolTipTextOn = toolTipTextOn;
    m_iconPathOff = ICONS_PATH + iconNameOff;
    m_toolTipTextOff = toolTipTextOff;

    setCheckable(true);
    setChecked(state);

    setStyleSheet(
        "QPushButton {"
        "   background-color: white;"
        "}"
        "QPushButton:checked {"
        "   background-color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: white;"
        "}"
    );

    setIconSize(QSize(10,30));

    updateIcons(state);

    connect(this, &QPushButton::clicked, this, &ToolbarToggleButton::onButtonToggled);
}


void ToolbarToggleButton::updateIcons(bool checked)
{
    if (checked) {
        setIcon(QIcon(m_iconPathOn));
        setToolTip(m_toolTipTextOn);
    } else {
        setIcon(QIcon(m_iconPathOff));
        setToolTip(m_toolTipTextOff);
    }
}


void ToolbarToggleButton::onButtonToggled(bool checked)
{
    if (checked) {
        emit stateActivated();
    } else {
        emit stateDeactivated();
    }
}


void ToolbarToggleButton::setButtonState(bool state)
{
    setChecked(state);
    updateIcons(state);
}
