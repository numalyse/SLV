#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarToggleButton.h"
#include <QStyleHints>
#include <QGuiApplication>

ToolbarToggleButton::ToolbarToggleButton(
    QWidget *parent, 
    bool state,
    const QString &iconNameOn, 
    const QString &toolTipTextOn, 
    const QString &iconNameOff, 
    const QString &toolTipTextOff)  : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    m_iconPathOn = ICONS_PATH + iconNameOn;
    m_toolTipTextOn = toolTipTextOn;
    m_iconPathOff = ICONS_PATH + iconNameOff;
    m_toolTipTextOff = toolTipTextOff;

    setCheckable(true);
    setChecked(state);

    // setIconSize(QSize(12,30));
    if (iconSize() == QSize(512, 512))
    {
        setIconSize(QSize(16, 16));
    }
    setFixedSize(30, 30);

    QImage imgOn = QImage(m_iconPathOn);

    if (QGuiApplication::styleHints()->colorScheme() != Qt::ColorScheme::Dark) {
        for (int y = 0; y < imgOn.height(); ++y) {
            for (int x = 0; x < imgOn.width(); ++x) {
                QColor c = imgOn.pixelColor(x, y);

                if (c.alpha() > 0) {
                    c.setRgb(0, 0, 0);
                    c.setAlpha(255);
                    imgOn.setPixelColor(x, y, c);
                }
            }
        }
    }

    QImage gray_imgOn = QImage(m_iconPathOn);

    for (int y = 0; y < gray_imgOn.height(); ++y) {
        for (int x = 0; x < gray_imgOn.width(); ++x) {
            QColor c = gray_imgOn.pixelColor(x, y);

            if (c.alpha() > 0) {
                c.setRgb(64, 64, 64);
                gray_imgOn.setPixelColor(x, y, c);
            }
        }
    }

    QImage imgOff = QImage(m_iconPathOff);

    if (QGuiApplication::styleHints()->colorScheme() != Qt::ColorScheme::Dark) {
        for (int y = 0; y < imgOff.height(); ++y) {
            for (int x = 0; x < imgOff.width(); ++x) {
                QColor c = imgOff.pixelColor(x, y);

                if (c.alpha() > 0) {
                    c.setRgb(0, 0, 0);
                    c.setAlpha(255);
                    imgOff.setPixelColor(x, y, c);
                }
            }
        }
    }

    QImage gray_imgOff = QImage(m_iconPathOff);

    for (int y = 0; y < gray_imgOff.height(); ++y) {
        for (int x = 0; x < gray_imgOff.width(); ++x) {
            QColor c = gray_imgOff.pixelColor(x, y);

            if (c.alpha() > 0) {
                c.setRgb(64, 64, 64);
                gray_imgOff.setPixelColor(x, y, c);
            }
        }
    }

    QIcon normalIconOn((QPixmap::fromImage(imgOn)));
    QIcon grayIconOn(QPixmap::fromImage(gray_imgOn));
    QIcon normalIconOff((QPixmap::fromImage(imgOff)));
    QIcon grayIconOff(QPixmap::fromImage(gray_imgOff));

    setProperty("normalIconOn", normalIconOn);
    setProperty("grayIconOn", grayIconOn);
    setProperty("normalIconOff", normalIconOff);
    setProperty("grayIconOff", grayIconOff);

    updateIcons(state, true);

    connect(this, &QPushButton::clicked, this, &ToolbarToggleButton::onButtonToggled);
}

QIcon ToolbarToggleButton::updateIconColor(QString iconName){
    QPixmap pix(iconName);

    QImage img = pix.toImage();

    if (QGuiApplication::styleHints()->colorScheme() != Qt::ColorScheme::Dark)
    {
        for (int y = 0; y < img.height(); ++y)
        {
            for (int x = 0; x < img.width(); ++x)
            {
                QColor c = img.pixelColor(x, y);

                if (c.alpha() > 0)
                {
                    c.setRgb(0, 0, 0);
                    c.setAlpha(255);
                    img.setPixelColor(x, y, c);
                }
            }
        }
    }

    return QIcon(QPixmap::fromImage(img));
}


void ToolbarToggleButton::updateIcons(bool checked, bool enabled)
{
    if (checked) {
        setIcon(enabled ? property("normalIconOn").value<QIcon>() : property("grayIconOn").value<QIcon>());
        setToolTip(m_toolTipTextOn);
    } else {
        setIcon(enabled ? property("normalIconOff").value<QIcon>() : property("grayIconOff").value<QIcon>());
        setToolTip(m_toolTipTextOff);
    }
}

void ToolbarToggleButton::updateIcons(bool checked)
{
    updateIcons(checked, true);
}

void ToolbarToggleButton::toggleUpdateIcon()
{
    updateIcons(isChecked(), isEnabled());
}


void ToolbarToggleButton::onButtonToggled(bool checked)
{
    if (checked) {
        emit stateActivated();
    } else {
        emit stateDeactivated();
    }
}


void ToolbarToggleButton::setButtonState(bool state)
{
    setChecked(state);
    updateIcons(state, isEnabled());
}

void ToolbarToggleButton::setToggledIconFrame(bool framed)
{
    if(framed){
        QString color = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "rgba(240,240,240,1);" : "rgba(0,0,0,1);";

        bool isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
        QColor palbtnColor;
        QString palbtnColorStr;

#ifdef Q_OS_MAC
        QColor colorBtn = qApp->palette().color(QPalette::Button);
        QColor enhancedColor = colorBtn.lighter(150);
        palbtnColorStr = QString(enhancedColor.name());
#else
        palbtnColorStr = "palette(button)";

#endif

        setStyleSheet(
            "ToolbarToggleButton{"
            "   background-color: rgba(0,0,0,0);"
            "   border: none;"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:hover{"
            "   background-color: " + palbtnColorStr + ";"
            "   border: 1px solid " + palbtnColorStr + ";"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:checked{"
            // "   background-color: palette(button);"
            "   border: 1px solid " +
            color +
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:checked:hover{"
            // "   background-color: palette(button);"
            "   border: 1px solid " +
            color +
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:disabled{"
            "   border: none;"
            "}");
    }
}

void ToolbarToggleButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        updateIcons(isChecked(), isEnabled());
    }
    QPushButton::changeEvent(event);
}