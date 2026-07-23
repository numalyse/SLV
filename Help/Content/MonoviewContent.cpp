#include "MonoviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MonoviewContent::MonoviewContent(QWidget* parent)
    : CategoryBase("help_menu_classic_category", parent)
{
    QString openMedia = "help_menu_open_files_label";

    addSubcategory(
        openMedia,
        openmedia(openMedia)
    );

    addSubcategory(
        "Player",
        player(openMedia)
    );

    
}

QWidget* MonoviewContent::openmedia(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->setDescription("help_menu_open_files_content_1");
    widget->setImage("open_medias");

    widget->setDescription("help_menu_open_files_content_2");
    widget->setImage("open_medias_drag_drop");

    return widget;
}

QWidget* MonoviewContent::player(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);


    return widget;
}