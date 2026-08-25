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
    QString presentationTitle = "help_menu_presentation_label";
    QString openMedia = "help_menu_open_files_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

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

QWidget* MonoviewContent::presentation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Le mode mono-vue est le mode d'affichage par défaut du lecteur multimédias.\n"
            "Il permet de visualiser un seul média à la fois et d'utiliser différents outils pour travailler et interagir avec celui-ci. \n"
            "Sa barre d'outils peut être étendue afin d'accéder à des fonctionnalités supplémentaires. \n"
            "Parmi ces outils, il est notamment possible d'accéder à une timeline détaillée, permettant de naviguer plus précisément dans le média et de visualiser son déroulement."
            "Il est également possible d'accéder à un affichage en playlist, permettant d'enchaîner la lecture de plusieurs médias à la suite."
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

    return widget;
}

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

    widget->addSectionName("help_menu_formats_label");
    
    widget->getFormatsAvailables();

    return widget;
}

QWidget* MonoviewContent::player(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);


    return widget;
}

// QWidget* MonoviewContent::formats(const QString& subcategoryName)
// {
//     auto* widget = new ContentBase(this, categoryName(), subcategoryName);


//     return widget;
// }