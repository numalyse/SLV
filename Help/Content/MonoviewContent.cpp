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

// QMap<QString, QString> createQMAP(QList<QString> list){
//      QMap<QString, QString> title = {
//         {"fr", ""},
//         {"en", ""},
//         {"es", ""},
//         {"de", ""},
//         {"it", ""},
//         {"pt", ""}
//     };


// }

QWidget* MonoviewContent::openmedia(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    //widget->addTextFromLangJSON("help_menu_open_files_content_1");

    widget->addTextFromLangQMAP({
        {"fr", 
            "Il existe différentes façon d'ouvrir un média. \n"
            "Faire cela."
        },
        {"en", 
            ""
        },
        {"es", 
            ""
        },
        {"de", 
            ""
        },
        {"it", 
            ""
        },
        {"pt", 
            ""
        }
    });

    //widget->addTextFromLangQMAP(title);

    widget->addImage("open_medias");

    widget->addTextFromLangJSON("help_menu_open_files_content_2");
    widget->addImage("open_medias_drag_drop");

    widget->addSectionName("help_menu");

    return widget;
}

QWidget* MonoviewContent::player(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);


    return widget;
}