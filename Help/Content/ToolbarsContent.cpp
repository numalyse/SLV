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

    //widget->addTextFromLangJSON("help_menu_classic_toolbar_content_1");

    widget->addTextFromLangQMAP({
        {"fr", 
            "La barre d'outils classique est composée de plusieurs boutons permettant d'accéder à différentes fonctionnalités.\n"
            "Elle est divisée en trois parties : gauche, centrale et droite.\n"
            "La partie gauche contient les boutons liés au son et aux informations du média.\n"
            "La partie centrale contient les boutons de lecture et de contrôle du média.\n"
            "La partie droite contient les boutons liés à l'affichage et aux fonctionnalités avancées.\n"
        },
        {"en", 
            "The classic toolbar consists of several buttons providing access to different features.\n"
            "It is divided into three sections: left, center, and right.\n"
            "The left section contains buttons related to sound and media information.\n"
            "The center section contains playback and media control buttons.\n"
            "The right section contains buttons related to display and advanced features.\n"
        },
        {"es", 
            "La barra de herramientas clásica está compuesta por varios botones que permiten acceder a diferentes funciones.\n"
            "Está dividida en tres partes: izquierda, central y derecha.\n"
            "La parte izquierda contiene los botones relacionados con el sonido y la información del medio.\n"
            "La parte central contiene los botones de reproducción y control del medio.\n"
            "La parte derecha contiene los botones relacionados con la visualización y las funciones avanzadas.\n"
        },
        {"de", 
            "Die klassische Symbolleiste besteht aus mehreren Schaltflächen, über die verschiedene Funktionen aufgerufen werden können.\n"
            "Sie ist in drei Bereiche unterteilt: links, Mitte und rechts.\n"
            "Der linke Bereich enthält die Schaltflächen für Ton und Medieninformationen.\n"
            "Der mittlere Bereich enthält die Schaltflächen für Wiedergabe und Mediensteuerung.\n"
            "Der rechte Bereich enthält die Schaltflächen für die Anzeige und erweiterte Funktionen.\n"
        },
        {"it", 
            "La barra degli strumenti classica è composta da diversi pulsanti che consentono di accedere a varie funzionalità.\n"
            "È suddivisa in tre parti: sinistra, centrale e destra.\n"
            "La parte sinistra contiene i pulsanti relativi all'audio e alle informazioni sul contenuto multimediale.\n"
            "La parte centrale contiene i pulsanti di riproduzione e di controllo del contenuto multimediale.\n"
            "La parte destra contiene i pulsanti relativi alla visualizzazione e alle funzionalità avanzate.\n"
        },
        {"pt", 
            "A barra de ferramentas clássica é composta por vários botões que permitem aceder a diferentes funcionalidades.\n"
            "Está dividida em três partes: esquerda, central e direita.\n"
            "A parte esquerda contém os botões relacionados com o som e as informações do conteúdo multimédia.\n"
            "A parte central contém os botões de reprodução e de controlo do conteúdo multimédia.\n"
            "A parte direita contém os botões relacionados com a apresentação e as funcionalidades avançadas.\n"
        }
    });

    widget->addButtonDescriptionTable(
        "help_menu_classic_toolbar_left_buttons_label", 
        {
            "sound_on",
            "lang",
            "media_info"
        }
    );

    widget->addVSpacing(10);

    widget->addButtonDescriptionTable(
        "help_menu_classic_toolbar_central_buttons_label", 
        {
            "slow",
            "stop",
            "prev",
            "play",
            "pause",
            "next",
            "eject",
            "loop_off"
        }
    ); 

    widget->addVSpacing(10);

    widget->addButtonDescriptionTable(
        "help_menu_classic_toolbar_right_buttons_label", 
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

    widget->addTextFromLangQMAP({
        {"fr", 
            "La barre d'outils avancée permet d'accéder à des fonctionnalités plus techniques.\n"
            "Elle est composée de plusieurs boutons permettant d'effectuer des actions spécifiques sur le média.\n"
            "Parmi ces fonctionnalités, il est possible de réaliser des ajustements sur l'image, d'appliquer des filtres, de dessiner sur le média, de gérer les règles de composition, de naviguer dans les images et d'accéder à la timeline détaillée.\n"
            "Pour y accéder, cliquer sur l'icône de barre d'outils avancée dans la partie droite de la barre d'outils classique.\n"
        },
        {"en", 
            "The advanced toolbar provides access to more technical features.\n"
            "It consists of several buttons for performing specific actions on the media.\n"
            "These features include adjusting the image, applying filters, drawing on the media, managing composition guides, navigating through images, and accessing the detailed timeline.\n"
            "To access it, click the advanced toolbar icon in the right section of the classic toolbar.\n"
        },
        {"es", 
            "La barra de herramientas avanzada permite acceder a funciones más técnicas.\n"
            "Está compuesta por varios botones que permiten realizar acciones específicas sobre el contenido multimedia.\n"
            "Entre estas funciones se incluyen el ajuste de la imagen, la aplicación de filtros, el dibujo sobre el contenido multimedia, la gestión de las reglas de composición, la navegación por las imágenes y el acceso a la línea de tiempo detallada.\n"
            "Para acceder a ella, haga clic en el icono de la barra de herramientas avanzada situado en la parte derecha de la barra de herramientas clásica.\n"
        },
        {"de", 
            "Die erweiterte Symbolleiste ermöglicht den Zugriff auf technischere Funktionen.\n"
            "Sie besteht aus mehreren Schaltflächen, mit denen bestimmte Aktionen am Medium ausgeführt werden können.\n"
            "Zu diesen Funktionen gehören Bildanpassungen, das Anwenden von Filtern, das Zeichnen auf dem Medium, die Verwaltung von Kompositionshilfen, das Navigieren durch Bilder und der Zugriff auf die detaillierte Zeitleiste.\n"
            "Um darauf zuzugreifen, klicken Sie auf das Symbol der erweiterten Symbolleiste im rechten Bereich der klassischen Symbolleiste.\n"
        },
        {"it", 
            "La barra degli strumenti avanzata consente di accedere a funzionalità più tecniche.\n"
            "È composta da diversi pulsanti che permettono di eseguire azioni specifiche sul contenuto multimediale.\n"
            "Tra queste funzionalità è possibile regolare l'immagine, applicare filtri, disegnare sul contenuto multimediale, gestire le guide di composizione, navigare tra le immagini e accedere alla timeline dettagliata.\n"
            "Per accedervi, fare clic sull'icona della barra degli strumenti avanzata nella parte destra della barra degli strumenti classica.\n"
        },
        {"pt", 
            "A barra de ferramentas avançada permite aceder a funcionalidades mais técnicas.\n"
            "É composta por vários botões que permitem realizar ações específicas sobre o conteúdo multimédia.\n"
            "Entre estas funcionalidades, é possível ajustar a imagem, aplicar filtros, desenhar sobre o conteúdo multimédia, gerir as regras de composição, navegar pelas imagens e aceder à linha do tempo detalhada.\n"
            "Para aceder a esta barra, clique no ícone da barra de ferramentas avançada na parte direita da barra de ferramentas clássica.\n"
        },
    });


    widget->addButtonDescriptionTable(
        "none", 
        {
            "show_image",
            "adjustments",
            "draw",
            "compo_rule",
            "backward",
            "forward",
            "prev_frame",
            "next_frame",
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

    widget->addTextFromLangQMAP({
        {"fr", 
            "La barre d'outils de la timeline détaillée permet d'effectuer des actions spécifiques sur la timeline.\n"
            "Elle est composée de plusieurs boutons permettant de naviguer dans la timeline, de gérer les segments et les plans, d'accéder aux détails des plans, d'ajouter des annotations et d'exporter.\n"
        },
        {"en", 
            "The detailed timeline toolbar allows you to perform specific actions on the timeline.\n"
            "It consists of several buttons for navigating the timeline, managing segments and shots, accessing shot details, adding annotations, and exporting.\n"
        },
        {"es", 
            "La barra de herramientas de la línea de tiempo detallada permite realizar acciones específicas en la línea de tiempo.\n"
            "Está compuesta por varios botones que permiten navegar por la línea de tiempo, gestionar segmentos y planos, acceder a los detalles de los planos, añadir anotaciones y exportar.\n"
        },
        {"de", 
            "Die Symbolleiste der detaillierten Zeitleiste ermöglicht die Ausführung bestimmter Aktionen in der Zeitleiste.\n"
            "Sie besteht aus mehreren Schaltflächen zum Navigieren in der Zeitleiste, Verwalten von Segmenten und Einstellungen, Zugreifen auf die Details der Einstellungen, Hinzufügen von Anmerkungen und Exportieren.\n"
        },
        {"it", 
            "La barra degli strumenti della timeline dettagliata consente di eseguire azioni specifiche sulla timeline.\n"
            "È composta da diversi pulsanti che permettono di navigare nella timeline, gestire segmenti e inquadrature, accedere ai dettagli delle inquadrature, aggiungere annotazioni ed esportare.\n"
        },
        {"pt", 
            "A barra de ferramentas da linha do tempo detalhada permite realizar ações específicas na linha do tempo.\n"
            "É composta por vários botões que permitem navegar pela linha do tempo, gerir segmentos e planos, aceder aos detalhes dos planos, adicionar anotações e exportar.\n"
        }
    });


    widget->addButtonDescriptionTable(
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
            "add_annot",
            "export"
        }
    );

    return widget;
}