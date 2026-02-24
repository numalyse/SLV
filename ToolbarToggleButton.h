#ifndef TOOLBARTOGGLEBUTTON_H
#define TOOLBARTOGGLEBUTTON_H 

#include <QPushButton>

class ToolbarToggleButton : public QPushButton
{
Q_OBJECT

public:
    explicit ToolbarToggleButton(
        QWidget* parent = nullptr, 
        bool state = false,
        const QString& iconNameOn = "", 
        const QString& toolTipTextOn = "ToolTipOn",
        const QString& iconNameOff = "", 
        const QString& toolTipTextOff = "ToolTipOff"
    );


    inline static const QString ICONS_PATH = ":/icons/";

signals:
    void stateActivated();  // Emis quand le bouton est "On"
    void stateDeactivated(); // Emis quand le bouton est "Off"

private:
    QString m_iconPathOn;
    QString m_toolTipTextOn;
    QString m_iconPathOff;
    QString m_toolTipTextOff;

    void updateIcons(bool checked);

private slots:
    void onButtonToggled(bool checked);
    
};

#endif
