#ifndef FORMCOMBOBOXFRAME_H
#define FORMCOMBOBOXFRAME_H

#include "Preference/BasePreferenceFrame.h"

#include <QStringList>
#include <QComboBox>

class FormComboBoxFrame : public BasePreferenceFrame
{

public:
    explicit FormComboBoxFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, const QStringList &values, QWidget *parent = nullptr);

private:
    QComboBox* m_comboBox = nullptr;

    

    void setUIValue(const QString &value);
};

#endif