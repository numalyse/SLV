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
    QString openMedia = "help_menu_open_file_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

    addSubcategory(
        openMedia,
        openmedia(openMedia)
    );

    // addSubcategory(
    //     "Player",
    //     player(openMedia)
    // );

    
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
            "Le mode mono-vue est le mode d'affichage par défaut du lecteur multimédia.\n"
            "Il permet de visualiser un seul média à la fois et d'utiliser différents outils pour travailler et interagir avec celui-ci. \n"
            "Sa barre d'outils peut être étendue afin d'accéder à des fonctionnalités supplémentaires. \n"
            "Parmi ces outils, il est notamment possible d'accéder à une timeline détaillée, permettant de naviguer plus précisément dans le média et de visualiser son déroulement.\n"
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
            "Pour ouvrir un média, plusieurs méthodes sont disponibles.\n"
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

    widget->addQMAPTexts({
        {"fr", {"Depuis le menu Fichier",
            "Cliquer sur la barre de menu « Fichier », sélectionner « Ouvrir un média… » pour parcourir les fichiers disponibles sur l'ordinateur. Sélectionner ensuite le média souhaité pour l'ouvrir dans l'application.\n"}
        },
        {"en", {"",
            ""}
        },
        {"es", {"",
            ""}
        },
        {"de", {"",
            ""}
        },
        {"it", {"",
            ""}
        },
        {"pt", {"",
            ""}
        }
    });

    widget->addImage("open_medias");

    widget->addQMAPTexts({
        {"fr", {"Par glisser-déposer",
            "Un média peut également être ouvert directement par glisser-déposer. Faire glisser le fichier depuis son emplacement sur l'ordinateur, puis le déposer dans la fenêtre de l'application. Le média est alors automatiquement chargé.\n"}
        },
        {"en", {"",
            ""}
        },
        {"es", {"",
            ""}
        },
        {"de", {"",
            ""}
        },
        {"it", {"",
            ""}
        },
        {"pt", {"",
            ""}
        }
    });

    widget->addImage("open_medias_drag_drop");

    widget->addQMAPTexts({
        {"fr", {"Avec le bouton Lecture",
            "Lorsqu'aucun média n'est déjà ouvert, le bouton Lecture permet également d'ouvrir un fichier. Cliquer sur ce bouton pour afficher le sélecteur de fichiers, puis choisir le média à ouvrir.\n"}
        },
        {"en", {"",
            ""}
        },
        {"es", {"",
            ""}
        },
        {"de", {"",
            ""}
        },
        {"it", {"",
            ""}
        },
        {"pt", {"",
            ""}
        }
    });

        widget->addImage("open_medias_play_button");

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