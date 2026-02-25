#ifndef TOOLBARTOGGLEHOVERBUTTON_H
#define TOOLBARTOGGLEHOVERBUTTON_H 

#include "ToolbarToggleButton.h"

#include <QPushButton>
#include <QTimer>

class ToolbarToggleHoverButton: public ToolbarToggleButton
{
Q_OBJECT

public:
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
    QTimer* m_hideTimer = nullptr; 

    void moveWidgetOnTop();

protected:
    void enterEvent(QEnterEvent * event) override;
    void leaveEvent(QEvent * event) override;
    bool eventFilter(QObject *watched, QEvent *event) override; // Pour catch les event d'un autre widget, ici m_widgetToDisplay


};

#endif