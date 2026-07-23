#include "GeneralContent.h"

#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>

GeneralContent::GeneralContent(QWidget* parent)
    : CategoryBase("General", parent)
{
    addSubcategory(
        "Introduction",
        introduction()
    );

    addSubcategory(
        "Player",
        player()
    );
}

QWidget* GeneralContent::introduction()
{
    auto* widget = new QWidget;

    //widget->addContent(new QLabel("Bienvenue dans la documentation."));

    return widget;
}

QWidget* GeneralContent::player()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Test"));

    return widget;
}