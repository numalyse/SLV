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
        "help_menu_classic_toolbar_buttons_descriptions",
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
    QWidget* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* buttonsDescriptionTable = new ContentBase;

    layout->addWidget(buttonsDescriptionTable->createButtonDescriptionTable(
        "help_menu_classic_toolbar_left_buttons", {
        {":/icons/sound_on", "help_menu_volume_label", "help_menu_volume_description"},
        {":/icons/lang", "help_menu_language_label", "help_menu_language_description"},
        {":/icons/media_info", "help_menu_media_info_label", "help_menu_media_info_description"}
    }));

    layout->addWidget(buttonsDescriptionTable->createButtonDescriptionTable(
        "help_menu_classic_toolbar_central_buttons", {
        {":/icons/slow", "help_menu_speed_label", "help_menu_speed_description"},
        {":/icons/play", "help_menu_play_label", "help_menu_play_description"},
        {":/icons/pause", "help_menu_pause_label", "help_menu_pause_description"},
        {":/icons/stop", "help_menu_stop_label", "help_menu_stop_description"},
        {":/icons/eject", "help_menu_eject_label", "help_menu_eject_description"},
        {":/icons/loop_off", "help_menu_loop_label", "help_menu_loop_description"},
    }));

    layout->addWidget(buttonsDescriptionTable->createButtonDescriptionTable(
        "help_menu_classic_toolbar_right_buttons", {
        {":/icons/zoom", "help_menu_zoom_label", "help_menu_zoom_description"},
        {":/icons/capture", "help_menu_screenshot_label", "help_menu_screenshot_description"},
        {":/icons/extract_sequence", "help_menu_extraction_label", "help_menu_extraction_description"},
        {":/icons/duplicate_media", "help_menu_duplicate_label", "help_menu_duplicate_description"},
        {":/icons/fullscreen", "help_menu_fullscreen_label", "help_menu_fullscreen_description"},
        {":/icons/right_arrow", "help_menu_extended_label", "help_menu_extended_description"},
    }));

    //layout->addWidget(buttonsDescriptionTable);

    return widget;

}