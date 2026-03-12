#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>


ToolbarToggleHoverButton::ToolbarToggleHoverButton(QWidget *parent, QWidget *widgetToDisplay, bool state, const QString &iconNameOn, const QString &toolTipTextOn, const QString &iconNameOff, const QString &toolTipTextOff, int timerDuration)
: ToolbarToggleButton(parent, state, iconNameOn, toolTipTextOn , iconNameOff, toolTipTextOff)
{
    Q_ASSERT(widgetToDisplay != nullptr);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(timerDuration);

    widgetToDisplay->setParent(nullptr);
    m_widgetToDisplay = widgetToDisplay;

    
    m_widgetToDisplay->installEventFilter(this);

    // Quand le timer est terminé, lance la méthode Qt hide de la popup.
    connect(m_hideTimer, &QTimer::timeout, m_widgetToDisplay, &QWidget::hide);
        
    moveWidgetOnTop();
    m_widgetToDisplay->hide();
}



void ToolbarToggleHoverButton::moveWidgetOnTop()
{
    m_widgetToDisplay->adjustSize(); // Oblige Qt à calculer la taille réelle du widget en fonction de son contenu (sinon lors du premier enter, la position est incorrect)

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int width = this->width();
    int height = this->height();

    int height_offset = 10;

    int x = globalPos.x() + (width/2) - (m_widgetToDisplay->width()/2);
    int y = globalPos.y() - m_widgetToDisplay->height() /*+ height_offset*/;

    m_widgetToDisplay->move(x, y);
}


void ToolbarToggleHoverButton::enterEvent(QEnterEvent *event)
{
    m_hideTimer->stop();
    moveWidgetOnTop();
    m_widgetToDisplay->show();

    ToolbarToggleButton::enterEvent(event);
}


void ToolbarToggleHoverButton::leaveEvent(QEvent *event)
{
    m_hideTimer->start();
    
    ToolbarToggleButton::leaveEvent(event);
}


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
