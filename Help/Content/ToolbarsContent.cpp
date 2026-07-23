#include "ToolbarsContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

ToolbarsContent::ToolbarsContent(QWidget* parent)
    : CategoryBase("help_menu_toolbars_label", parent)
{
    QString classicToolbarName = "help_menu_classic_toolbar_label";
    QString extendedToolbarName = "help_menu_extended_toolbar_label";
    QString timelineToolbarName = "help_menu_timeline_toolbar_label";

    addSubcategory(
        classicToolbarName,
        classicToolbar(classicToolbarName)
    );

    addSubcategory(
        extendedToolbarName,
        extendedToolbar(extendedToolbarName)
    );

    addSubcategory(
        timelineToolbarName,
        timelineToolbar(timelineToolbarName)
    );

    
}

QWidget* ToolbarsContent::classicToolbar(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->setDescription("help_menu_classic_toolbar_content_1");

    widget->createButtonDescriptionTable(
        "help_menu_classic_toolbar_left_buttons", 
        {
            "sound_on",
            "lang",
            "media_info"
        }
    );

    widget->createButtonDescriptionTable(
        "help_menu_classic_toolbar_central_buttons", 
        {
            "slow",
            "play",
            "pause",
            "stop",
            "eject",
            "loop_off"
        }
    );

    widget->createButtonDescriptionTable(
        "help_menu_classic_toolbar_right_buttons", 
        {
            "zoom",
            "capture",
            "extract_sequence",
            "duplicate_media",
            "fullscreen",
            "right_arrow"
        }
    );

    return widget;
}

QWidget* ToolbarsContent::extendedToolbar(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->createButtonDescriptionTable(
        "none", 
        {
            "show_image",
            "adjustments",
            "draw",
            "compo_rule",
            "backward",
            "prev_frame",
            "record_off",
            "rotate",
            "invert_h",
            "timeline_off"
        }
    );

    return widget;
}

QWidget* ToolbarsContent::timelineToolbar(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->createButtonDescriptionTable(
        "none", 
        {
            "abloop",
            "auto_segmentation",
            "split_shot",
            "merge_left",
            "to_prev_shot",
            "shot_detail",
            "to_next_shot",
            "merge_right",
            "open_annot",
            "plus",
            "export"
        }
    );

    return widget;
}