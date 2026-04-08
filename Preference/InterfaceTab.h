#ifndef INTERFACETAB_H
#define INTERFACETAB_H

#include <QWidget>
#include <QJsonObject>
#include <QVector>

class InterfaceTab : public QWidget
{
Q_OBJECT

public:
    explicit InterfaceTab(QWidget* parent = nullptr);

    bool needSave();

    void save();

    void discard();

    ~InterfaceTab() {}

private:
    QJsonObject m_baseInterface;
    QJsonObject m_updatedInterface;

    QVector<std::function<void()>> m_discardActions; // for now uses this to discard changes, each widgets add its reset function

};


#endif