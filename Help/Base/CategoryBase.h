#ifndef CATEGORYBASE_H
#define CATEGORYBASE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>

class QVBoxLayout;
class QPushButton;

class CategoryBase : public QWidget
{
    Q_OBJECT

public:
    explicit CategoryBase(const QString& categoryName,
                          QWidget* parent = nullptr);

    void addSubcategory(const QString& name, QWidget* content);

    void populateTree(QTreeWidget* tree);

    QString categoryName() const;

signals:

private:
    struct Subcategory
    {
        QString name;
        QWidget* content;
    };

    QString m_categoryName;
    QVBoxLayout* m_layout;
    QList<Subcategory> m_subcategories;

};

#endif