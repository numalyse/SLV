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
    explicit ContentBase(QWidget *parent, const QString& categoryName, const QString& subcategoryName);

    void addContent(QWidget* widget);
    void addLayout(QLayout *layout);

    void setCategoryName(const QString &categoryName);
    void setsubcategoryName(const QString &subcategoryName);

    QWidget *createTable(const QString& tableName, const QList<QWidget *> &rows);
    QWidget *createButtonDescription(const QString &iconName, const QString &buttonLabel, const QString &buttonDescription);
    void createButtonDescriptionTable(const QString& tableName, std::initializer_list<QString> button);

    void setImage(const QString &imageName);
    void setDescription(const QString &descriptionName);

protected:
    QVBoxLayout* m_layout;
    PrefManager& pref;
    QString theme;
    QString backgroundFillColor;

    int m_imageWidth;


};

#endif