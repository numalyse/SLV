#include "CategoryBase.h"

#include <QVBoxLayout>

CategoryBase::CategoryBase(const QString& categoryName, QWidget* parent)
    : QWidget(parent), m_categoryName(categoryName) , pref(PrefManager::instance())
{
    pref = PrefManager::instance();
    theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : ""; 

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);
    m_layout->setAlignment(Qt::AlignTop);
}

void CategoryBase::addSubcategory(const QString& name, QWidget* content)
{
    m_subcategories.append({
        pref.getText(name),
        content
    });
}

void CategoryBase::populateTree(QTreeWidget* tree)
{
    auto* categoryItem = new QTreeWidgetItem(tree);

    categoryItem->setText(0, pref.getText(m_categoryName));

    for(auto subcategory : m_subcategories)
    {
        auto* subcategoryItem = new QTreeWidgetItem(categoryItem);

        subcategoryItem->setText(0, subcategory.name);
        subcategoryItem->setData(0, Qt::UserRole, QVariant::fromValue(subcategory.content));
    }

}

QString CategoryBase::categoryName() const
{
    return m_categoryName;
}

