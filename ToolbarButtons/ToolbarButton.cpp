#include "ToolbarButtons/ToolbarButton.h"
#include "IconHelper.h"
#include <QEvent>
#include <QStyleHints>
#include <QGuiApplication>

ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    if(iconName == ""){
        setText("Icon");
    }else {
        QPixmap pix(IconHelper::themedIconPath(iconName));

        QIcon normalIcon(pix);
        QIcon grayIcon(QPixmap(IconHelper::grayIconPath(iconName)));

        setProperty("normalIcon", normalIcon);
        setProperty("grayIcon", grayIcon);

        setIcon(normalIcon);

        // Par défaut redimension auto à QSize(16,16)
        if(pix.size() == QSize(512,512) || pix.size() == QSize(128,128)){
            setIconSize(QSize(16, 16));
        }
        // qDebug() << iconName << " - taille img : " << img.size();
        // qDebug() << iconName << " - taille icon : " << iconSize();
        setFixedSize(30, 30);
        // setStyleSheet("background-color: palette(button);");

    }
    setToolTip(toolTipText);
}

void ToolbarButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        setIcon(isEnabled()
            ? property("normalIcon").value<QIcon>()
            : property("grayIcon").value<QIcon>());
    }
    QPushButton::changeEvent(event);
}

