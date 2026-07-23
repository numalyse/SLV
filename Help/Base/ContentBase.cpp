#include "ContentBase.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QVector>
#include <QHeaderView>
#include <QLabel>
#include <QScrollArea>

ContentBase::ContentBase(QWidget *parent, const QString& categoryName, const QString& subcategoryName)
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

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 

    //m_imageWidth = (scrollArea->viewport()->width() - 30) * 0.85;
    m_imageWidth = 800;

    QWidget* content = new QWidget();
    m_layout = new QVBoxLayout(content);

    setsubcategoryName(subcategoryName);
    setCategoryName(categoryName);

    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(10);
    m_layout->addStretch();

    scrollArea->setWidget(content);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);

}

void ContentBase::addContent(QWidget* widget)
{
    if (!widget)
        return;

    const int nbWidget = m_layout->count();
    const int index = (nbWidget > 0) ? nbWidget - 1 : 0;

    m_layout->insertWidget(index, widget);
}

void ContentBase::addLayout(QLayout* layout)
{
    m_layout->addLayout(layout);
}

void ContentBase::setCategoryName(const QString& categoryName){
    auto* name = new QLabel(pref.getText(categoryName));

    QFont nameFont = name->font();
    nameFont.setPointSize(20);
    nameFont.setBold(true);
    name->setFont(nameFont);

    addContent(name);
}

void ContentBase::setsubcategoryName(const QString& subcategoryName){
    auto* name = new QLabel(pref.getText(subcategoryName));

    QFont nameFont = name->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    name->setFont(nameFont);

    addContent(name);
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
        //row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(row);
    }

    return widget;
}

QWidget* ContentBase::createButtonDescription(const QString& iconName, const QString& buttonLabel, const QString& buttonDescription)
{
    auto* widget = new QWidget(this);
    widget->setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 1px; border-radius: 5px;");

    auto* layout = new QHBoxLayout(widget);
    //layout->setContentsMargins(20, 20, 20, 20);

    auto* icon = new QLabel();
    icon->setPixmap(QPixmap(iconName + theme).scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //icon->setStyleSheet("padding: 1px");
    icon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* label = new QLabel("<b>" + pref.getText(buttonLabel) + "</b>");
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* description = new QLabel(pref.getText(buttonDescription));
    description->setWordWrap(true);
    description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->addWidget(icon, 1, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(label, 2);
    layout->addSpacing(20);
    layout->addWidget(description, 6);

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    widget->adjustSize();

    return widget;
}

void ContentBase::createButtonDescriptionTable(const QString& tableName, std::initializer_list<QString> buttons)
{
    QList<QWidget*> rows;

    for (const auto& button : buttons)
    {
        rows << createButtonDescription(
            ":/icons/" + button,
            "help_menu_" + button + "_label",
            "help_menu_" + button + "_description"
        );
    }

    addContent(createTable(tableName, rows));
}

void ContentBase::setImage(const QString& imageName)
{
    auto* widget = new QWidget(this);
    auto* layout = new QHBoxLayout(widget);

    QLabel* illustrationLabel = new QLabel();
    QPixmap illustration(":/help_dialog_illustrations/"+imageName);
    illustrationLabel->setPixmap(illustration.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    ));

    illustrationLabel->setAlignment(Qt::AlignCenter);
    illustrationLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    layout->addWidget(illustrationLabel);

    addContent(widget);
}

void ContentBase::setDescription(const QString& descriptionName)
{
    auto* widget = new QWidget(this);
    auto* layout = new QHBoxLayout(widget);

    QLabel* descriptionLabel = new QLabel(pref.getText(descriptionName));
    
    layout->addWidget(descriptionLabel);

    addContent(widget);
}
