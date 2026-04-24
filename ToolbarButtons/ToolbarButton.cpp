#include "ToolbarButtons/ToolbarButton.h"
#include <QEvent>
#include <QStyleHints>
#include <QGuiApplication>

ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    if(iconName == ""){
        setText("Icon");
    }else {
        QPixmap pix(ICONS_PATH + iconName);

        QImage img = pix.toImage();

        if (QGuiApplication::styleHints()->colorScheme() != Qt::ColorScheme::Dark) {
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor c = img.pixelColor(x, y);

                    if (c.alpha() > 0) {
                        c.setRgb(0, 0, 0);
                        c.setAlpha(255);
                        img.setPixelColor(x, y, c);
                    }
                }
            }
        } 

        QImage gray_img = pix.toImage();

        for (int y = 0; y < gray_img.height(); ++y) {
            for (int x = 0; x < gray_img.width(); ++x) {
                QColor c = gray_img.pixelColor(x, y);

                if (c.alpha() > 0) {
                    c.setRgb(64, 64, 64);
                    gray_img.setPixelColor(x, y, c);
                }
            }
        }

        //QIcon normalIcon(pix);
        QIcon normalIcon(QPixmap::fromImage(img));
        QIcon grayIcon(QPixmap::fromImage(gray_img));

        setProperty("normalIcon", normalIcon);
        setProperty("grayIcon", grayIcon);

        setIcon(normalIcon);

        //setIcon(QIcon(ICONS_PATH + iconName));
        // setIconSize(QSize(12,30));
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

