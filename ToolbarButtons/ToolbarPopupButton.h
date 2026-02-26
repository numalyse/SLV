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

    void moveWidgetOnTop();

private slots:
    void displayPopup();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;


};

#endif