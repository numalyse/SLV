#ifndef BASEPREFERENCETAB_H
#define BASEPREFERENCETAB_H

#include "Preference/BasePreferenceFrame.h"

#include <QScrollArea>
#include <QJsonObject>
#include <QList>
#include <QFormLayout>

/// @brief Base for all of the preference tab, contains a scroll pane by default, ease the addition of new tabs and centralize methods
class BasePreferenceTab : public QScrollArea
{
Q_OBJECT

public:
    explicit BasePreferenceTab(const QString& categoryName, QWidget* parent = nullptr);
    virtual ~BasePreferenceTab() = default;

    /// @brief Checks whether a tab was modified 
    bool needSave() const;

    /// @brief Save its data to user pref json
    virtual void save();

    /// @brief Reverts all changes, puts all frames back to its original values
    virtual void discard();

signals:
    void tabChanges();

protected slots:
    /// @brief updates the json object, if subcategory -> key : value failed, try to do key : value. If this also fails only writes qdebug message
    virtual void updateJsonObj(const QString& subCategory, const QString& key, const QString& newValue);

protected:
    QString m_categoryName;      
    QJsonObject m_baseJson;
    QJsonObject m_updatedJson;
    
    QList<BasePreferenceFrame*> m_frames; 

    QWidget* m_container;
    QFormLayout* m_layout;

    /// @brief Helper to add to the tab the frame by usnig addRow, also conencts its updateJsonObjRequested signal to updateJsonObj slot
    void addPreferenceFrame(BasePreferenceFrame* frame);
};

#endif // BASEPREFERENCETAB_H