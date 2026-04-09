#ifndef FORMPATHEDITFRAME_H
#define FORMPATHEDITFRAME_H

#include "Preference/BasePreferenceFrame.h"

class QLabel;
class QPushButton;

class FormPathEditFrame : public BasePreferenceFrame 
{
    Q_OBJECT

public:
    FormPathEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, bool isFolder = false, QWidget *parent = nullptr);

    void clearPathUI();
    void setUIValue(const QString &path);

private:
    QLabel* m_pathLabel = nullptr;
    QPushButton* m_browseBtn = nullptr;
};

#endif