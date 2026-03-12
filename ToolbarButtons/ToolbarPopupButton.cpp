#include "ToolbarButtons/ToolbarPopupButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>


ToolbarPopupButton::ToolbarPopupButton(QWidget *parent, QWidget *widgetToDisplay, const QString &iconName, const QString &toolTipText) : ToolbarButton(parent, iconName, toolTipText)
{
    Q_ASSERT(widgetToDisplay != nullptr);

    widgetToDisplay->setParent(nullptr);
    m_widgetToDisplay = widgetToDisplay;
    m_widgetToDisplay->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);// Le widget ne sera plus dans une fenêtre séparé et sera au dessus des autres widgets
    
    m_widgetToDisplay->installEventFilter(this);
    m_widgetToDisplay->hide();

    connect(this, &QPushButton::clicked, this, &ToolbarPopupButton::displayPopup);    
}


void ToolbarPopupButton::displayPopup(){
    if (m_blockNextShow){
        m_blockNextShow = false;
        return;
    }

    moveWidgetOnTop();
    m_widgetToDisplay->show();
}


void ToolbarPopupButton::moveWidgetOnTop()
{
    m_widgetToDisplay->adjustSize(); // Oblige Qt à calculer la taille réelle du widget en fonction de son contenu (sinon lors du premier enter, la position est incorrect)

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int width = this->width();
    // int height = this->height();

    int height_offset = 10;

    int x = globalPos.x() + (width/2) - (m_widgetToDisplay->width()/2);
    int y = globalPos.y() - m_widgetToDisplay->height() /*+ height_offset*/;

    m_widgetToDisplay->move(x, y);
}


bool ToolbarPopupButton::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_widgetToDisplay) {
        if (event->type() == QEvent::Hide ) {
            QPoint mousePos = QCursor::pos(); 
            // Check si la souris est contenu dans le bouton lorsqu'on hide, 
            // si True : c'est un clique sur le bouton pendant que la popup était visible. Ce flag va empecher à la pop up de se réafficher 
            if (this->rect().contains(this->mapFromGlobal(mousePos))) {
                m_blockNextShow = true;
            }
        }
        
    }

    return ToolbarButton::eventFilter(watched, event);
}
