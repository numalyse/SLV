#include "TimelineContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

TimelineContent::TimelineContent(QWidget* parent)
    : CategoryBase("help_menu_timeline_category", parent)
{    
    QString presentationTitle = "help_menu_presentation_label";
    QString openTimeline = "help_menu_open_playlist_label";
    QString multiview = "help_menu_enter_timeline_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );
    
    addSubcategory(
        openTimeline,
        introduction(openTimeline)
    );

    
}

QWidget* TimelineContent::presentation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Cette fonctionnalité est accessible uniquement en mode mono-vue.\n"
            "L'affichage de la timeline détaillée offre une réprésentation plus précise du déroulement du média, avec davantage d'informations qu'une barre de progession classique.\n"
            "Une barre d'outils l'accompagne afin de travailler sur le média et de pouvoir diviser celui-ci en plusieurs plans.\n"
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

QWidget* TimelineContent::introduction(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "\n"
            "\n"
            "\n"
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

