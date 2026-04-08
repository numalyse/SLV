#ifndef INTERFACETAB_H
#define INTERFACETAB_H

#include <QWidget>
#include <QJsonObject>
#include <QVector>
#include <QScrollArea>

class InterfaceTab : public QScrollArea
{
Q_OBJECT

public:
    explicit InterfaceTab(QWidget* parent = nullptr);

    /// @brief Checks whether one of the pref has been modified and needs to ask user to save
    /// @return True if needs save
    bool needSave();

    /// @brief Updates the json with all of the changes from this tab
    void save();

    /// @brief Reverts all changes of all tabs
    void discard();

    ~InterfaceTab() {}

private:
    // use of 2 json object to check if needing to save and be able to discard changes
    QJsonObject m_baseInterface;
    QJsonObject m_updatedInterface;

    QVector<std::function<void()>> m_discardActions; // for now uses this to discard changes, each widgets add its reset function

};


#endif