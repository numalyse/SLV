#include "ContentBase.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QVector>
#include <QHeaderView>
#include <QLabel>

ContentBase::ContentBase(QWidget *parent)
    : QWidget(parent) , pref(PrefManager::instance())
{
    pref = PrefManager::instance();
    theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : ""; 
    
#ifdef Q_OS_MAC
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        backgroundFillColor = "palette(mid)";
    } else {
        backgroundFillColor = "palette(base)";
    }
#else
    backgroundFillColor = "palette(base)";
#endif

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

QWidget* ContentBase::createTable(const QString& tableName, const QList<QWidget*>& rows)
{
    auto* widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* layout = new QVBoxLayout(widget);
    
    if(pref.getText(tableName) != "[none]"){
        auto* titleTable = new QLabel(pref.getText(tableName));
        layout->addWidget(titleTable);
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    for (QWidget* row : rows)
    {
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(row);
    }

    return widget;
}

QWidget* ContentBase::createButtonDescriptionTable(const QString& tableName, std::initializer_list<ButtonDescriptionData> buttons)
{
    QList<QWidget*> rows;

    for (const auto& button : buttons)
    {
        rows << createButtonDescription(
            button.icon,
            button.label,
            button.description
        );
    }

    return createTable(tableName, rows);
}

QWidget* ContentBase::createButtonDescription(const QString& iconName, const QString& buttonLabel, const QString& buttonDescription)
{
    auto* widget = new QWidget(this);
    widget->setMinimumHeight(20);
    widget->setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 1px; border-radius: 5px;");

    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(20, 20, 20, 20);

    auto* icon = new QLabel();
    icon->setPixmap(QPixmap(iconName + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto* label = new QLabel("<b>" + pref.getText(buttonLabel) + "</b>");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* description = new QLabel(pref.getText(buttonDescription));
    description->setWordWrap(true);
    description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->addWidget(icon, 1);
    layout->addSpacing(20);
    layout->addWidget(label, 1);
    layout->addSpacing(20);
    layout->addWidget(description, 6);

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    widget->adjustSize();

    return widget;
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

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);

    addContent(table);
}

TableRow ContentBase::addButtonDescription(const QString& iconName, const QString& buttonLabel, const QString buttonDescription)
{
    TableRow rowButton;

    QLabel* extendedIcon = new QLabel();
    QLabel* extendedLabel = new QLabel("<b>" + pref.getText(buttonLabel) + "</b>");
    QLabel* extendedDescription = new QLabel(pref.getText(buttonDescription));

    rowButton.append(extendedIcon);
    rowButton.append(extendedLabel);
    rowButton.append(extendedDescription);

    return rowButton;
}

