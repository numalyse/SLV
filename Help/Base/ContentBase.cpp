#include "ContentBase.h"

#include "PrefManager.h"
#include <QGuiApplication>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QVector>
#include <QHeaderView>
#include <QLabel>

ContentBase::ContentBase(QWidget *parent)
    : QWidget(parent)
{
    PrefManager pref = PrefManager::instance();
    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : ""; 

    m_layout = new QVBoxLayout(this);

    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(10);

}

void ContentBase::addContent(QWidget* widget)
{
    if (widget)
        m_layout->addWidget(widget);
}

void ContentBase::addLayout(QLayout* layout)
{
    m_layout->addLayout(layout);
}

void ContentBase::addTable(const QVector<TableRow>& rows)
{
    auto* table = new QTableWidget(this);

    if(rows.isEmpty())
        return;

    const int columnCount = rows.first().size();

    table->setColumnCount(columnCount);
    table->setRowCount(rows.size());

    for(int row = 0; row < rows.size(); ++row)
    {
        const auto& columns = rows[row];

        for(int col = 0; col < columns.size(); ++col)
        {
            table->setCellWidget(
                row,
                col,
                columns[col]
            );
        }
    }

    table->horizontalHeader()
          ->setSectionResizeMode(QHeaderView::Stretch);

    table->verticalHeader()->setVisible(false);

    addContent(table);
}

TableRow ContentBase::addButtonDescription(const QString& iconName, const QString& buttonLabel, const QString buttonDescription)
{
    TableRow rowButton;

    QLabel* extendedIcon = new QLabel();
    QLabel* extendedLabel = new QLabel("<b>" + buttonLabel + "</b>");
    QLabel* extendedDescription = new QLabel(buttonDescription);

    rowButton.append(extendedIcon);
    rowButton.append(extendedLabel);
    rowButton.append(extendedDescription);

    return rowButton;
}
