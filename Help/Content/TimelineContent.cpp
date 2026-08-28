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
    QString openTimelineTitle = "help_menu_open_timeline_label";
    QString openshotDetailTitle = "help_menu_open_info_label";
    QString exportationTitle = "help_menu_timeline_exportation_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );
    
    addSubcategory(
        openTimelineTitle,
        openTimeline(openTimelineTitle)
    );

    addSubcategory(
        openshotDetailTitle,
        shotDetail(openshotDetailTitle)
    );

    addSubcategory(
        exportationTitle,
        exportation(exportationTitle)
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

    widget->addImage("timeline_display");

    widget->addTextFromLangQMAP({
        {"fr", 
            "3 barres sont présentes sur la timeline :\n"
            "Une barre d'annotations, une barre réprésentant l'image (affichage d'une vignette représentant le début du plan) et une barre pour le son (onde sonore). \n"
            "Celles-ci sont dimensionnées par une échelle de temps, permettant de visualiser le déroulement du média et de naviguer plus précisément.\n"
            "Par défaut, le média est réprésenté par un plan unique. Il est possible d'ajouter des plans en segmentant automatique ou manuellement celui-ci.\n"
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

QWidget* TimelineContent::openTimeline(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Pour ouvrir la timeline avancée, il faut cliquer sur l'icône de la timeline dans la barre d'outils avancée.\n"
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

QWidget* TimelineContent::shotDetail(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Pour accéder aux détails d'un plan, il faut cliquer sur l'icône de détail d'un plan dans la barre d'outils de la timeline ou via le bouton d'ouverture du panneau latéral (en haut à droite).\n"
            "Cet affichage permet de visualiser les informations détaillées sur le plan sélectionné : numéro, début, fin, et durée du plan.\n"
            "Une autre section permet à l'utilisateur d'ajouter du contenu en donnant un titre ainsi que des commentaires sur l'image ou le son.\n"
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

    widget->addImage("timeline_shot_infos");

    return widget;
}

QWidget* TimelineContent::exportation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Pour exporter la timeline, il faut cliquer sur l'icône d'exportation dans la barre d'outils de la timeline.\n"
            "Cette fonctionnalité permet d'exporter le média avec les plans et les commentaires ajoutés.\n"
            "L'utilisateur peut choisir le format d'exportation ainsi que le nom et l'emplacement du fichier exporté.\n"
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

    widget->addImage("timeline_exportation");

    return widget;
}