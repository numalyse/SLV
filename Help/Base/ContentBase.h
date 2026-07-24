#ifndef CONTENTBASE_H
#define CONTENTBASE_H

#include "PrefManager.h"
#include "FileFormatManager.h"
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
    void addSectionName(const QString &sectionName);

    QWidget *createTable(const QString& tableName, const QList<QWidget *> &rows);
    QWidget *createButtonDescription(const QString &iconName, const QString &buttonLabel, const QString &buttonDescription);
    void addButtonDescriptionTable(const QString& tableName, std::initializer_list<QString> button);

    void addImage(const QString &imageName);
    void addTextFromLangJSON(const QString &descriptionName);
    void addTextFromLangQMAP(QMap<QString, QString> texts);

    void addMails(const QStringList &mails);

protected:
    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_contentLayout;
    PrefManager& pref;
    FileFormatManager& format;
    QString theme;
    QString backgroundFillColor;

    int m_imageWidth;


};

#endif