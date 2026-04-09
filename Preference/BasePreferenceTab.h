#ifndef BASEPREFERENCETAB_H
#define BASEPREFERENCETAB_H

#include "Preference/BasePreferenceFrame.h"

#include <QScrollArea>
#include <QJsonObject>
#include <QList>

class QFormLayout;

class BasePreferenceTab : public QScrollArea
{
    Q_OBJECT

public:
    explicit BasePreferenceTab(const QString& categoryName, QWidget* parent = nullptr);
    virtual ~BasePreferenceTab() = default;

    bool needSave() const;
    virtual void save();
    virtual void discard();

signals:
    void tabChanges();

protected slots:
    virtual void updateJsonObj(const QString& subCategory, const QString& key, const QString& newValue);

protected:
    QString m_categoryName;      
    QJsonObject m_baseJson;
    QJsonObject m_updatedJson;
    
    QList<BasePreferenceFrame*> m_frames; 

    QWidget* m_container;
    QFormLayout* m_layout;

    void addPreferenceFrame(BasePreferenceFrame* frame);
};

#endif // BASEPREFERENCETAB_H