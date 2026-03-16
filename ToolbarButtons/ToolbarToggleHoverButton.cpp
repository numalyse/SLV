#include "ToolbarButtons/ToolbarToggleHoverButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>
#include <qapplication.h>
#include <qframe.h>
#include <qlayout.h>
#include <qscreen.h>


ToolbarToggleHoverButton::ToolbarToggleHoverButton(QWidget *parent, QLayout *layoutToDisplay, bool state, const QString &iconNameOn, const QString &toolTipTextOn, const QString &iconNameOff, const QString &toolTipTextOff, int timerDuration)
: ToolbarToggleButton(parent, state, iconNameOn, toolTipTextOn , iconNameOff, toolTipTextOff)
{
    Q_ASSERT(layoutToDisplay != nullptr);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(timerDuration);

    QWidget* container = new QWidget();
    layoutToDisplay->setParent(nullptr);
    container->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
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
        "QLabel {"
        " border: none;"
        "}"
        );
    containerLayout->addWidget(containerBackground);

    layoutToDisplay->setContentsMargins(6,6,6,6);
    containerBackground->setLayout(layoutToDisplay);

    m_widgetToDisplay = container;

    
    m_widgetToDisplay->installEventFilter(this);

    // Quand le timer est terminé, lance la méthode Qt hide de la popup.
    connect(m_hideTimer, &QTimer::timeout, m_widgetToDisplay, &QWidget::hide);
        
    moveWidget();
    m_widgetToDisplay->hide();
}

void ToolbarToggleHoverButton::setOnTop(const bool &onTop)
{
    m_onTop = onTop;
}

void ToolbarToggleHoverButton::moveWidget()
{
    m_widgetToDisplay->adjustSize();

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int x = globalPos.x() + (width() / 2) - (m_widgetToDisplay->width() / 2);
    int y = globalPos.y() - m_widgetToDisplay->height();
    if(!m_onTop){
        y = globalPos.y() + height() + 6;
    }

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


void ToolbarToggleHoverButton::enterEvent(QEnterEvent *event)
{
    m_hideTimer->stop();
    moveWidget();
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
