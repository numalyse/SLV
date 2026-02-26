#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>

/// @brief Créer un bouton toggle qui affiche une popup quand on le survole
/// @param parent 
/// @param widgetToDisplay 
/// @param state 
/// @param iconNameOn 
/// @param toolTipTextOn 
/// @param iconNameOff 
/// @param toolTipTextOff 
/// @param timerDuration Durée pendant laquelle la popup reste affiché après être sortie du survol du bouton ou de la popup
ToolbarToggleHoverButton::ToolbarToggleHoverButton(QWidget *parent, QWidget *widgetToDisplay, bool state, const QString &iconNameOn, const QString &toolTipTextOn, const QString &iconNameOff, const QString &toolTipTextOff, int timerDuration)
: ToolbarToggleButton(parent, state, iconNameOn, toolTipTextOn , iconNameOff, toolTipTextOff)
{
    Q_ASSERT(widgetToDisplay != nullptr);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(timerDuration);

    widgetToDisplay->setParent(nullptr);
    m_widgetToDisplay = widgetToDisplay;
    m_widgetToDisplay->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    
    m_widgetToDisplay->installEventFilter(this);

    // Quand le timer est terminé, lance la méthode Qt hide de la popup.
    connect(m_hideTimer, &QTimer::timeout, m_widgetToDisplay, &QWidget::hide);
        
    moveWidgetOnTop();
    m_widgetToDisplay->hide();
}


/// @brief Déplace le widget popup centré au dessus du bouton, offset sur la hauteur fixé a 10 px
void ToolbarToggleHoverButton::moveWidgetOnTop()
{
    m_widgetToDisplay->adjustSize(); // Oblige Qt à calculer la taille réelle du widget en fonction de son contenu (sinon lors du premier enter, la position est incorrect)

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int width = this->width();
    int height = this->height();

    int height_offset = 10;

    int x = globalPos.x() + (width/2) - (m_widgetToDisplay->width()/2);
    int y = globalPos.y() - m_widgetToDisplay->height() + height_offset;

    m_widgetToDisplay->move(x, y);
}

/// @brief Lorsque la souris survole le bouton, arret du timer et show de la popup
/// @param event 
void ToolbarToggleHoverButton::enterEvent(QEnterEvent *event)
{
    m_hideTimer->stop();
    moveWidgetOnTop();
    m_widgetToDisplay->show();

    ToolbarToggleButton::enterEvent(event);
}

/// @brief Lorsque la souris quitte le bouton, lance le timer (lorsque le timer se termine, cache la popup).
/// @param event 
void ToolbarToggleHoverButton::leaveEvent(QEvent *event)
{
    m_hideTimer->start();
    
    ToolbarToggleButton::leaveEvent(event);
}

/// @brief Quand la souris entre dans la popup arrete le timer, quand le souris sort de la popup, lance le timer (lorsque le timer se termine, cache la popup).
/// @param watched m_widgetToDisplay
/// @param event
/// @return 
bool ToolbarToggleHoverButton::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_widgetToDisplay) {
        if (event->type() == QEvent::Enter) {
            m_hideTimer->stop(); 
        } else if (event->type() == QEvent::Leave) {
            m_hideTimer->start(); 
        }
    }

    return ToolbarToggleButton::eventFilter(watched, event);
}
