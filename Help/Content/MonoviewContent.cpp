#include "MonoviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MonoviewContent::MonoviewContent(QWidget* parent)
    : CategoryBase("Monoview", parent)
{


    addSubcategory(
        "Introduction",
        introduction()
    );

    addSubcategory(
        "Player",
        player()
    );

    addSubcategory(
        "Description",
        classicBar()
    );

    
}

QWidget* MonoviewContent::introduction()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Bienvenue dans la documentation!!!!"));

    return widget;
}

QWidget* MonoviewContent::player()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Test"));

    return widget;
}

QWidget* MonoviewContent::classicBar()
{
    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : ""; 
    PrefManager pref = PrefManager::instance();

    auto* widget = new ContentBase;

    QVector<TableRow> rows;

    rows.append(widget->addButtonDescription(":/icons/slow" + theme, pref.getText("help_menu_speed_label"), pref.getText("help_menu_speed_description")));
    rows.append(widget->addButtonDescription(":/icons/loop_off" + theme, pref.getText("help_menu_loop_label"), pref.getText("help_menu_loop_description")));
    rows.append(widget->addButtonDescription(":/icons/sound_on" + theme, pref.getText("help_menu_volume_label"), pref.getText("help_menu_volume_description")));
    rows.append(widget->addButtonDescription(":/icons/lang" + theme, pref.getText("help_menu_language_label"), pref.getText("help_menu_language_description")));
    rows.append(widget->addButtonDescription(":/icons/media_info" + theme, pref.getText("help_menu_media_info_label"), pref.getText("help_menu_media_info_description")));
    rows.append(widget->addButtonDescription(":/icons/zoom" + theme, pref.getText("help_menu_zoom_label"), pref.getText("help_menu_zoom_description")));
    rows.append(widget->addButtonDescription(":/icons/capture" + theme, pref.getText("help_menu_screenshot_label"), pref.getText("help_menu_screenshot_description")));
    rows.append(widget->addButtonDescription(":/icons/extract_sequence" + theme, pref.getText("help_menu_extraction_label"), pref.getText("help_menu_extraction_description")));
    rows.append(widget->addButtonDescription(":/icons/duplicate_media" + theme, pref.getText("help_menu_duplicate_label"), pref.getText("help_menu_duplicate_description")));
    rows.append(widget->addButtonDescription(":/icons/fullscreen" + theme, pref.getText("help_menu_fullscreen_label"), pref.getText("help_menu_fullscreen_description")));
    rows.append(widget->addButtonDescription(":/icons/right_arrow" + theme, pref.getText("help_menu_extended_label"), pref.getText("help_menu_extended_description")));

    widget->addTable(rows);

    return widget;
}