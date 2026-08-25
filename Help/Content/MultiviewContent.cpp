#include "MultiviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MultiviewContent::MultiviewContent(QWidget* parent)
    : CategoryBase("help_menu_multiview_category", parent)
{
    QString presentationTitle = "help_menu_presentation_label";
    QString multiview = "help_menu_open_multiview_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

    addSubcategory(
        multiview,
        introduction(multiview)
    );

    
}

QWidget* MultiviewContent::presentation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Le mode multi-vue permet de visualiser jusqu'à 4 médias à la fois au sein de la même interface.\n"
            "Une barre de navigation supplémentaire s'ajoute à l'interface et permet de contrôler tous les médias simultanément.\n"
            "Il offre la possibilité de visualiser différents contenus en parallèle, facilitant ainsi leur comparaison, leur suivi ou leur consultation simultanée. Chaque vue peut être utilisée indépendamment, tout en conservant un accès aux principaux outils de lecture et de navigation. \n"
            "L'affichage peut être adapté selon les besoins afin d'organiser les médias de manière claire et de profiter pleinement de l'espace disponible.\n"
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

QWidget* MultiviewContent::introduction(const QString& subcategoryName)
{
    QWidget* widget = new ContentBase(this, categoryName(), subcategoryName);

    return widget;
}
