#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H 

#include <QPushButton>

class ToolbarButton : public QPushButton
{
Q_OBJECT

public:
    explicit ToolbarButton(QWidget* parent = nullptr, const QString& iconName = "", const QString& toolTipText = "ToolTip");

    inline static const QString ICONS_PATH = ":/icons/";
    
private:
    
};

#endif
