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
    QString duplicate = "help_menu_duplicate_file_label";
    QString manage = "help_menu_manage_multiview_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

    addSubcategory(
        multiview,
        openMultiviewMode(multiview)
    );

    addSubcategory(
        duplicate,
        duplicateFile(duplicate)
    );

    addSubcategory(
        manage,
        manageMultiview(manage)
    );

}

QWidget* MultiviewContent::presentation(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Le mode multi-vue permet de visualiser jusqu'à 4 médias à la fois au sein de la même interface.\n"
            "Une barre de navigation supplémentaire s'ajoute à l'interface et permet de contrôler tous les médias simultanément.\n"
            "Il offre la possibilité de visualiser différents contenus en parallèle, facilitant ainsi leur consultation ou leur comparaison simultanée. Chaque vue peut être utilisée indépendamment, tout en conservant un accès aux principaux outils de lecture et de navigation. \n"
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

    widget->addImage("multiview_display");

    widget->addTextFromLangQMAP({
        {"fr", 
            "Deux méthodes permettent d'afficher plusieurs médias au sein du Numalyse Player. \n"
            "Leur différence repose sur la façon dont elles agissent.\n"
            "Le premier permet d'ajouter ou supprimer plusieurs lecteurs à la fois tandis que le second duplique un lecteur existant et son contenu.\n"
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

QWidget* MultiviewContent::openMultiviewMode(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addQMAPTexts({
        {"fr", {"Via l'icône de mode d'affichage",
            "Pour accéder rapidement au mode multi-vues, en haut à droite de l'interface, survoler l'icône de redisposition des lecteurs.\n"
            "Glisser la souris puis cliquer sur l'icône correspondante à la disposition désirée. "
            "(En mode 2 ou 3 vues, il est possible de choisir celle-ci.)\n"
            "Un ou plusieurs lecteurs seront ajoutés ou supprimés si le nombre de vues est modifié.\n"}
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

    widget->addImages({"multiview_change_arrangement", "multiview_change_arrangement_2"});

    return widget;
}

QWidget* MultiviewContent::duplicateFile(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);
    widget->addQMAPTexts({
        {"fr", {"",
            "Cliquer sur l'icône de duplication d'un lecteur. Le média sera ainsi dupliqué dans une nouvelle vue en conservant ses paramètres.\n"
            "Cette méthode permet de visualiser le même média dans plusieurs lecteurs et de les contrôler indépendamment.\n"
            "Ainsi, il est possible de comparer différentes parties du média ou d'appliquer des réglages distincts à chaque vue.\n"
            "Cette fonctionnalité peut être particulièrement utile pour l'analyse détaillée d'un même média.\n\n"
            "Attention : Le lecteur dupliqué a une disposition par défaut si elle par deux ou trois."
            "\n"}
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

    return widget;
}



QWidget* MultiviewContent::manageMultiview(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addQMAPTexts({
        {"fr", {"Barre de navigation individuelle",
            "Une barre de lecture simplifiée permet de gérer chaque lecteur indépendamment. Elle offre les fonctionnalités essentielles pour contrôler la lecture de chaque média.\n"
            "\n"}
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

    widget->addImage("multiview_simplified_toolbar");

    widget->addQMAPTexts({
        {"fr", {"Barre de navigation commune",
            "Une barre de navigation commune permet de gérer tous les lecteurs simultanément.\n"
            "\n"}
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

    widget->addImage("multiview_global_toolbar");

    widget->addQMAPTexts({
        {"fr", {"Capture d'écran multiple",
            "Le bouton de capture de la barre commune permet de prendre une capture d'écran de chaque média et les combine entre elles en une seule image en gardant la disposition des lecteurs.\n"
            "Les captures d'écrans sont enregistrées dans le dossier choisi par l'utilisateur dans les paramètres de préférence. (NumalysePlayer_Content par défaut)\n"}
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

    widget->addImage("multiview_captures");
    
    return widget;
}

