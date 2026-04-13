#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H 

#include <QPushButton>

class ToolbarButton : public QPushButton
{
Q_OBJECT

public:

    /// @brief Créer un bouton avec une icone et un tooltip
    /// @param parent 
    /// @param iconName nom de l'icone ex : "pause.png"
    /// @param toolTipText Le texte du tooltip
    explicit ToolbarButton(QWidget* parent = nullptr, const QString& iconName = "", const QString& toolTipText = "ToolTip");

    inline static const QString ICONS_PATH = ":/icons/";

protected:
    void changeEvent(QEvent *event) override;

};

#endif
