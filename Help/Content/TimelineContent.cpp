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
            "This feature is only accessible in single-view mode.\n"
            "The detailed timeline view offers a more precise representation of the media's progress, with more information than a classic progress bar.\n"
            "A toolbar accompanies it to work on the media and to be able to split it into several shots.\n"
        },
        {"es", 
            "Esta función solo está disponible en modo de vista única.\n"
            "La visualización de la línea de tiempo detallada ofrece una representación más precisa del desarrollo del medio, con más información que una barra de progreso clásica.\n"
            "Una barra de herramientas la acompaña para poder trabajar sobre el medio y dividirlo en varios planos.\n"
        },
        {"de", 
            "Diese Funktion ist nur im Einzelansichtsmodus verfügbar.\n"
            "Die detaillierte Timeline-Ansicht bietet eine präzisere Darstellung des Medienverlaufs, mit mehr Informationen als eine klassische Fortschrittsleiste.\n"
            "Eine Werkzeugleiste begleitet sie, um am Medium zu arbeiten und es in mehrere Einstellungen unterteilen zu können.\n"
        },
        {"it", 
            "Questa funzionalità è accessibile solo in modalità vista singola.\n"
            "La visualizzazione della timeline dettagliata offre una rappresentazione più precisa dell'andamento del contenuto multimediale, con maggiori informazioni rispetto a una barra di avanzamento classica.\n"
            "Una barra degli strumenti la accompagna per poter lavorare sul contenuto multimediale e suddividerlo in più inquadrature.\n"
        },
        {"pt", 
            "Esse recurso está disponível apenas no modo de exibição única.\n"
            "A exibição da linha do tempo detalhada oferece uma representação mais precisa do andamento da mídia, com mais informações do que uma barra de progresso clássica.\n"
            "Uma barra de ferramentas a acompanha para permitir trabalhar na mídia e dividi-la em vários planos.\n"
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
            "3 bars are present on the timeline:\n"
            "An annotation bar, a bar representing the image (displaying a thumbnail showing the beginning of the shot), and a bar for the sound (sound wave).\n"
            "These are sized according to a time scale, allowing you to view the progress of the media and navigate more precisely.\n"
            "By default, the media is represented by a single shot. Shots can be added by segmenting it automatically or manually.\n"
        },
        {"es", 
            "En la línea de tiempo hay 3 barras:\n"
            "Una barra de anotaciones, una barra que representa la imagen (que muestra una miniatura del inicio del plano) y una barra para el sonido (onda sonora).\n"
            "Estas se dimensionan según una escala de tiempo, lo que permite visualizar el desarrollo del medio y navegar con mayor precisión.\n"
            "Por defecto, el medio está representado por un único plano. Es posible añadir planos segmentándolo automática o manualmente.\n"
        },
        {"de", 
            "Auf der Timeline sind 3 Leisten vorhanden:\n"
            "Eine Anmerkungsleiste, eine Leiste, die das Bild darstellt (Anzeige einer Miniaturansicht des Einstellungsanfangs), und eine Leiste für den Ton (Tonwelle).\n"
            "Diese sind entsprechend einer Zeitskala dimensioniert, wodurch der Ablauf des Mediums sichtbar wird und eine präzisere Navigation ermöglicht wird.\n"
            "Standardmäßig wird das Medium durch eine einzige Einstellung dargestellt. Es ist möglich, Einstellungen hinzuzufügen, indem man es automatisch oder manuell segmentiert.\n"
        },
        {"it", 
            "Sulla timeline sono presenti 3 barre:\n"
            "Una barra delle annotazioni, una barra che rappresenta l'immagine (visualizzazione di una miniatura che mostra l'inizio dell'inquadratura) e una barra per l'audio (onda sonora).\n"
            "Queste sono dimensionate secondo una scala temporale, che consente di visualizzare lo svolgimento del contenuto multimediale e di navigare in modo più preciso.\n"
            "Per impostazione predefinita, il contenuto multimediale è rappresentato da un'unica inquadratura. È possibile aggiungere inquadrature segmentandolo automaticamente o manualmente.\n"
        },
        {"pt", 
            "3 barras estão presentes na linha do tempo:\n"
            "Uma barra de anotações, uma barra que representa a imagem (exibindo uma miniatura que mostra o início do plano) e uma barra para o som (onda sonora).\n"
            "Estas são dimensionadas de acordo com uma escala de tempo, permitindo visualizar o andamento da mídia e navegar com mais precisão.\n"
            "Por padrão, a mídia é representada por um único plano. É possível adicionar planos segmentando-o automática ou manualmente.\n"
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
            "To open the advanced timeline, click on the timeline icon in the advanced toolbar.\n"
            "\n"
            "\n"
        },
        {"es", 
            "Para abrir la línea de tiempo avanzada, haga clic en el icono de la línea de tiempo en la barra de herramientas avanzada.\n"
            "\n"
            "\n"
        },
        {"de", 
            "Um die erweiterte Timeline zu öffnen, klicken Sie auf das Timeline-Symbol in der erweiterten Werkzeugleiste.\n"
            "\n"
            "\n"
        },
        {"it", 
            "Per aprire la timeline avanzata, fare clic sull'icona della timeline nella barra degli strumenti avanzata.\n"
            "\n"
            "\n"
        },
        {"pt", 
            "Para abrir a linha do tempo avançada, clique no ícone da linha do tempo na barra de ferramentas avançada.\n"
            "\n"
            "\n"
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
            "To access the details of a shot, click on the shot detail icon in the timeline toolbar or via the side panel opening button (top right).\n"
            "This view lets you see the detailed information about the selected shot: number, start, end, and duration of the shot.\n"
            "Another section lets the user add content by giving a title as well as comments on the image or sound.\n"
        },
        {"es", 
            "Para acceder a los detalles de un plano, haga clic en el icono de detalle de un plano en la barra de herramientas de la línea de tiempo o mediante el botón de apertura del panel lateral (arriba a la derecha).\n"
            "Esta vista permite visualizar la información detallada del plano seleccionado: número, inicio, fin y duración del plano.\n"
            "Otra sección permite al usuario añadir contenido indicando un título así como comentarios sobre la imagen o el sonido.\n"
        },
        {"de", 
            "Um auf die Details einer Einstellung zuzugreifen, klicken Sie auf das Detailsymbol einer Einstellung in der Timeline-Werkzeugleiste oder über die Schaltfläche zum Öffnen des Seitenpanels (oben rechts).\n"
            "Diese Ansicht zeigt die detaillierten Informationen zur ausgewählten Einstellung: Nummer, Anfang, Ende und Dauer der Einstellung.\n"
            "Ein weiterer Bereich ermöglicht es dem Benutzer, Inhalte hinzuzufügen, indem er einen Titel sowie Kommentare zu Bild oder Ton angibt.\n"
        },
        {"it", 
            "Per accedere ai dettagli di un'inquadratura, fare clic sull'icona dei dettagli di un'inquadratura nella barra degli strumenti della timeline oppure tramite il pulsante di apertura del pannello laterale (in alto a destra).\n"
            "Questa visualizzazione consente di visualizzare le informazioni dettagliate sull'inquadratura selezionata: numero, inizio, fine e durata dell'inquadratura.\n"
            "Un'altra sezione consente all'utente di aggiungere contenuti indicando un titolo e commenti sull'immagine o sull'audio.\n"
        },
        {"pt", 
            "Para acessar os detalhes de um plano, clique no ícone de detalhe de um plano na barra de ferramentas da linha do tempo ou através do botão de abertura do painel lateral (no canto superior direito).\n"
            "Essa exibição permite visualizar as informações detalhadas sobre o plano selecionado: número, início, fim e duração do plano.\n"
            "Outra seção permite ao usuário adicionar conteúdo fornecendo um título, bem como comentários sobre a imagem ou o som.\n"
        }
    });

    widget->addImage("shot");

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
            "To export the timeline, click on the export icon in the timeline toolbar.\n"
            "This feature lets you export the media with the added shots and comments.\n"
            "The user can choose the export format as well as the name and location of the exported file.\n"
        },
        {"es", 
            "Para exportar la línea de tiempo, haga clic en el icono de exportación en la barra de herramientas de la línea de tiempo.\n"
            "Esta función permite exportar el medio con los planos y los comentarios añadidos.\n"
            "El usuario puede elegir el formato de exportación así como el nombre y la ubicación del archivo exportado.\n"
        },
        {"de", 
            "Um die Timeline zu exportieren, klicken Sie auf das Export-Symbol in der Timeline-Werkzeugleiste.\n"
            "Mit dieser Funktion können Sie das Medium zusammen mit den hinzugefügten Einstellungen und Kommentaren exportieren.\n"
            "Der Benutzer kann das Exportformat sowie den Namen und den Speicherort der exportierten Datei wählen.\n"
        },
        {"it", 
            "Per esportare la timeline, fare clic sull'icona di esportazione nella barra degli strumenti della timeline.\n"
            "Questa funzionalità consente di esportare il contenuto multimediale con le inquadrature e i commenti aggiunti.\n"
            "L'utente può scegliere il formato di esportazione, nonché il nome e la posizione del file esportato.\n"
        },
        {"pt", 
            "Para exportar a linha do tempo, clique no ícone de exportação na barra de ferramentas da linha do tempo.\n"
            "Esse recurso permite exportar a mídia com os planos e comentários adicionados.\n"
            "O usuário pode escolher o formato de exportação, bem como o nome e o local do arquivo exportado.\n"
        }
    });

    widget->addImage("export_project");

    return widget;
}