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
    setFixedSize(30, 30);

    updateIcons(state);

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


void ToolbarToggleButton::updateIcons(bool checked)
{
    if (checked) {
        //setIcon(QIcon(m_iconPathOn));
        setIcon(updateIconColor(m_iconPathOn));
        setToolTip(m_toolTipTextOn);
    } else {
        //setIcon(QIcon(m_iconPathOff));
        setIcon(updateIconColor(m_iconPathOff));
        setToolTip(m_toolTipTextOff);
    }
}

void ToolbarToggleButton::toggleUpdateIcon()
{
    if(isChecked()){
        //setIcon(QIcon(m_iconPathOn));
        setIcon(updateIconColor(m_iconPathOn));
        setToolTip(m_toolTipTextOn);
    }
    else{
        //setIcon(QIcon(m_iconPathOff));
        setIcon(updateIconColor(m_iconPathOff));
        setToolTip(m_toolTipTextOff);
    }
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
    updateIcons(state);
}

void ToolbarToggleButton::setToggledIconFrame(bool framed)
{
    if(framed){
        setStyleSheet(
            "ToolbarToggleButton{"
            "   background-color: rgba(0,0,0,0);"
            "   border: none;"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:hover{"
            "   background-color: palette(button);"
            "   border: 1px solid palette(button);"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:checked{"
            // "   background-color: palette(button);"
            "   border: 1px solid rgba(240,240,240,1);"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:checked:hover{"
            // "   background-color: palette(button);"
            "   border: 1px solid rgba(240,240,240,1);"
            "   border-radius: 4px;"
            "}"
            "ToolbarToggleButton:disabled{"
            "   border: none;"
            "}"
        );
    }
}
