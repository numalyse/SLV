#include "ToolbarButtons/ToolbarButton.h"
#include <QEvent>


ToolbarButton::ToolbarButton(QWidget *parent, const QString &iconName, const QString &toolTipText) : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    if(iconName == ""){
        setText("Icon");
    }else {
        QPixmap pix(ICONS_PATH + iconName);

        QImage img = pix.toImage();

        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                QColor c = img.pixelColor(x, y);

                if (c.alpha() > 0) {
                    c.setRgb(64, 64, 64);
                    img.setPixelColor(x, y, c);
                }
            }
        }

        QIcon normalIcon(pix);
        QIcon grayIcon(QPixmap::fromImage(img));

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

