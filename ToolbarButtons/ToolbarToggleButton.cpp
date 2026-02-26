#include "ToolbarButtons/ToolbarToggleButton.h"


/// @brief Créer un bouton qui change d'icone quand il est cliqué, comportement similaire à un QPushButton checkable.
/// @brief Emet un signal lorsqu'il passe "On" : stateActivated ou "Off" : stateDeactivated
/// @param parent QWidget* parent
/// @param state bool : L'état par défaut 
/// @param iconNameOn QString : L'icone à afficher quand coché ou "On"
/// @param toolTipTextOn QString : Le tooltip à afficher quand coché ou "On"
/// @param iconNameOff QString : L'icone à afficher quand décoché ou "Off"
/// @param toolTipTextOff QString : L'icone à afficher quand décoché ou "Off"
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

/// @brief Met à jour les icones en fonction de l'état
/// @param checked bool
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

/// @brief Slot privé appelé lorsque le bouton est cliqué. Met à jour les icônes puis emet le signal "On" ou "Off"
/// @param checked bool
void ToolbarToggleButton::onButtonToggled(bool checked)
{
    updateIcons(checked);

    if (checked) {
        emit stateActivated();
    } else {
        emit stateDeactivated();
    }
}