#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarToggleButton.h"
#include "IconHelper.h"
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
    m_toolTipTextOn = toolTipTextOn;
    m_toolTipTextOff = toolTipTextOff;

    setCheckable(true);
    setChecked(state);

    // setIconSize(QSize(12,30));
    if (iconSize() == QSize(512, 512))
    {
        setIconSize(QSize(16, 16));
    }
    setFixedSize(30, 30);

    QIcon normalIconOn(QPixmap(IconHelper::themedIconPath(iconNameOn)));
    QIcon grayIconOn(QPixmap(IconHelper::grayIconPath(iconNameOn)));
    QIcon normalIconOff(QPixmap(IconHelper::themedIconPath(iconNameOff)));
    QIcon grayIconOff(QPixmap(IconHelper::grayIconPath(iconNameOff)));

    setProperty("normalIconOn", normalIconOn);
    setProperty("grayIconOn", grayIconOn);
    setProperty("normalIconOff", normalIconOff);
    setProperty("grayIconOff", grayIconOff);

    updateIcons(state, true);

    connect(this, &QPushButton::clicked, this, &ToolbarToggleButton::onButtonToggled);
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