#include "PlaylistContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

PlaylistContent::PlaylistContent(QWidget* parent)
    : CategoryBase("help_menu_playlist_category", parent)
{    
    QString presentationTitle = "help_menu_presentation_label";
    QString openPlaylist = "help_menu_open_playlist_label";
    QString managePlaylistTitle = "help_menu_manage_playlist_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

    addSubcategory(
        managePlaylistTitle,
        managePlaylist(managePlaylistTitle)
    );



    // TO DO :
    // - basic (add, remove, delete)
    // - advanced (shuffle, loop, ordering)
    
}

QWidget* PlaylistContent::presentation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Cette fonctionnalité est accessible uniquement en mode mono-vue.\n"
            "La playlist permet de lire permet d'organiser plusieurs médias au sein d'une même liste de lecture et de les lire successivement.\n"
            "Elle offre ainsi une navigation simple entre les différents contenus et permet de gérer facilement l'ordre de lecture.\n"
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

    widget->addImage("playlist_display");

    return widget;
}

QWidget* PlaylistContent::managePlaylist(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Plusieurs fonctionnalités permettent d'ajouter, de supprimer et de réorganiser les médias au sein de la liste de lecture.\n"
            "Pour ajouter un média à la playlist, cliquer sur l'icône d'ajout et en sélectionnant le fichier souhaité, ou via un glisser-déposer.\n"
            "Pour supprimer un média, il suffit de cliquer sur l'icône représentée par une poubelle.\n"
            "Pour réorganiser l'ordre des médias, il est possible de les faire glisser à l'endroit souhaité à l'aide de la souris.\n"
            "Des boutons de lecture en boucle, en mode aléatoire et de tri se situent en haut de la playlist.\n"
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

    widget->addImage("playlist_display");

    return widget;
}