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

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
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

    return widget;
}
