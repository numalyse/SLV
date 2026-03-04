#ifndef TOOLBARTOGGLEBUTTON_H
#define TOOLBARTOGGLEBUTTON_H 

#include <QPushButton>

class ToolbarToggleButton : public QPushButton
{
Q_OBJECT

public:

    /// @brief Créer un bouton qui change d'icone quand il est cliqué, comportement similaire à un QPushButton checkable.
    /// @brief Emet un signal lorsqu'il passe "On" : stateActivated ou "Off" : stateDeactivated
    /// @param state bool : L'état par défaut 
    explicit ToolbarToggleButton(
        QWidget* parent = nullptr, 
        bool state = false,
        const QString& iconNameOn = "", 
        const QString& toolTipTextOn = "ToolTipOn",
        const QString& iconNameOff = "", 
        const QString& toolTipTextOff = "ToolTipOff"
    );

    /// @brief Modifie l'état du bouton sans avoir besoin d'utiliser les signaux
    /// @param state 
    void setButtonState(bool state);

    inline static const QString ICONS_PATH = ":/icons/";

signals:
    void stateActivated();  // Emis quand le bouton est "On"
    void stateDeactivated(); // Emis quand le bouton est "Off"

private:
    QString m_iconPathOn;
    QString m_toolTipTextOn;
    QString m_iconPathOff;
    QString m_toolTipTextOff;

protected:
    /// @brief Met à jour les icones en fonction de l'état
    void updateIcons(bool checked);



protected slots:
    /// @brief Appelé lorsque le bouton est cliqué. Met à jour les icônes puis emet le signal stateActivated ou stateDeactivated
    virtual void onButtonToggled(bool checked);

};

#endif
