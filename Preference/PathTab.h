#ifndef PATHTAB_H
#define PATHTAB_H

#include "Preference/BasePreferenceTab.h"

#include "PrefManager.h"
#include "Preference/FormPathEditFrame.h"

#include <QWidget>
#include <QJsonObject>
#include <QVector>
#include <QScrollArea>

class PathTab : public BasePreferenceTab
{    
Q_OBJECT

public:
    explicit PathTab(QWidget* parent = nullptr);

private:

};


#endif