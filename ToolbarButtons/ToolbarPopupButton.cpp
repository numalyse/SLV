#include "ToolbarButtons/ToolbarPopupButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qframe.h>
#include <qscreen.h>


ToolbarPopupButton::ToolbarPopupButton(QWidget *parent, QLayout *layoutToDisplay, const QString &iconName, const QString &toolTipText) : ToolbarButton(parent, iconName, toolTipText)
{
    Q_ASSERT(layoutToDisplay != nullptr);

    layoutToDisplay->setParent(nullptr);
    // m_widgetToDisplay = widgetToDisplay;
    QWidget* container = new QWidget();
    container->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    container->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(0,0,0,0);

    QFrame* containerBackground = new QFrame();
    containerBackground->setStyleSheet(
        "QFrame {"
        " background-color: palette(base);"
        " border-radius: 6px;"
        " border: 1px solid palette(button);"
        "}"
        );
    containerLayout->addWidget(containerBackground);

    layoutToDisplay->setContentsMargins(6,6,6,6);
    containerBackground->setLayout(layoutToDisplay);

    m_widgetToDisplay = container;
    
    m_widgetToDisplay->installEventFilter(this);
    m_widgetToDisplay->hide();

    connect(this, &QPushButton::clicked, this, &ToolbarPopupButton::displayPopup);    
}


void ToolbarPopupButton::displayPopup(){
    if (m_blockNextShow){
        m_blockNextShow = false;
        return;
    }

    moveWidget();
    m_widgetToDisplay->show();
}

void ToolbarPopupButton::setOnTop(const bool &onTop)
{
    m_onTop = onTop;
}

void ToolbarPopupButton::moveWidget()
{
    m_widgetToDisplay->adjustSize();

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int x = globalPos.x() + (width() / 2) - (m_widgetToDisplay->width() / 2);
    int y = globalPos.y() - m_widgetToDisplay->height();
    if(!m_onTop)
        y = globalPos.y() + height();

    QRect popupRect(x, y,
                    m_widgetToDisplay->width(),
                    m_widgetToDisplay->height());

    QScreen *screen = QGuiApplication::screenAt(globalPos);
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    QRect screenGeom = screen->availableGeometry();

    if (popupRect.left() < screenGeom.left())
        x = screenGeom.left() + 6;

    if (popupRect.right() > screenGeom.right())
        x = screenGeom.right() - popupRect.width() - 6;

    if (popupRect.top() < screenGeom.top())
        y = globalPos.y() + height() + 6;

    if (popupRect.bottom() > screenGeom.bottom())
        y = screenGeom.bottom() - popupRect.height() - 6;

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
