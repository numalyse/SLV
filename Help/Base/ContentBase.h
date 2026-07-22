#ifndef CONTENTBASE_H
#define CONTENTBASE_H

#include <QWidget>

class QVBoxLayout;
using TableRow = QVector<QWidget*>;

class ContentBase : public QWidget
{
    Q_OBJECT

public:
    explicit ContentBase(QWidget *parent = nullptr);

    void addContent(QWidget* widget);
    void addLayout(QLayout *layout);
    void addTable(const QVector<TableRow>& rows);

    TableRow addButtonDescription(const QString &iconName, const QString &buttonLabel, const QString buttonDescription);

protected:
    QVBoxLayout* m_layout;
};

#endif