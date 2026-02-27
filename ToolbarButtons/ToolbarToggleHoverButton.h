#ifndef TOOLBARTOGGLEHOVERBUTTON_H
#define TOOLBARTOGGLEHOVERBUTTON_H 

#include "ToolbarButtons/ToolbarToggleButton.h"

#include <QPushButton>
#include <QTimer>

class ToolbarToggleHoverButton: public ToolbarToggleButton
{
Q_OBJECT

public:


    /// @brief Créer un bouton toggle qui affiche une popup quand on le survole
    /// @param timerDuration Durée pendant laquelle la popup reste affiché après être sortie du survol du bouton ou de la popup
    explicit ToolbarToggleHoverButton(
        QWidget* parent = nullptr,
        QWidget* widgetToDisplay = nullptr,
        bool state = false,
        const QString& iconNameOn = "", 
        const QString& toolTipTextOn = "ToolTipOn",
        const QString& iconNameOff = "", 
        const QString& toolTipTextOff = "ToolTipOff",
        int timerDuration = 100
    );

    ~ToolbarToggleHoverButton() { delete m_widgetToDisplay; }; // Le widget n'a pas de parent, il doit être détruit manuellement

private:
    QWidget* m_widgetToDisplay = nullptr;

    /// @brief Timer qui cache la popup quand arrive à expiration
    QTimer* m_hideTimer = nullptr; 

    /// @brief Déplace le widget popup centré au dessus du bouton, offset sur la hauteur fixé a 10 px
    void moveWidgetOnTop();

protected:
    /// @brief Lorsque la souris survole le bouton, arret du timer et show de la popup
    void enterEvent(QEnterEvent * event) override;

    /// @brief Lorsque la souris quitte le bouton, lance le timer (lorsque le timer se termine, cache la popup).
    void leaveEvent(QEvent * event) override;

    /// @brief Quand la souris entre dans la popup arrete le timer, 
    /// quand le souris sort de la popup, lance le timer.
    bool eventFilter(QObject *watched, QEvent *event) override; 

};

#endif