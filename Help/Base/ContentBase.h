#ifndef CONTENTBASE_H
#define CONTENTBASE_H

#include "PrefManager.h"
#include <QWidget>
#include <QGuiApplication>
#include <QStyleHints>

class QVBoxLayout;
using TableRow = QVector<QWidget*>;

struct ButtonDescriptionData
{
    QString icon;
    QString label;
    QString description;
};

class ContentBase : public QWidget
{
    Q_OBJECT

public:
    explicit ContentBase(QWidget *parent = nullptr);

    void addContent(QWidget* widget);
    void addLayout(QLayout *layout);
    
    QWidget *createTable(const QString& tableName, const QList<QWidget *> &rows);
    QWidget* createButtonDescriptionTable(const QString& tableName, std::initializer_list<ButtonDescriptionData> buttons);
    QWidget *createButtonDescription(const QString &iconName, const QString &buttonLabel, const QString &buttonDescription);
    
    void addTable(const QVector<TableRow>& rows);
    TableRow addButtonDescription(const QString &iconName, const QString &buttonLabel, const QString buttonDescription);


protected:
    QVBoxLayout* m_layout;
    PrefManager& pref;
    QString theme;
    QString backgroundFillColor;


};

#endif