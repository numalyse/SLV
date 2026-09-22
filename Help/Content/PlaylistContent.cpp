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
            "La playlist permet d'organiser plusieurs médias au sein d'une même liste de lecture et de les lire successivement.\n"
            "Elle offre ainsi une navigation simple entre les différents contenus et permet de gérer facilement l'ordre de lecture.\n"
        },
        {"en", 
            "This feature is only available in single-view mode.\n"
            "The playlist allows you to organize several media within a single playback list and play them successively.\n"
            "It thus offers simple navigation between the different content and makes it easy to manage the playback order.\n"
        },
        {"es", 
            "Esta función solo está disponible en modo de vista única.\n"
            "La playlist permite organizar varios medios dentro de una misma lista de reproducción y reproducirlos sucesivamente.\n"
            "De este modo, ofrece una navegación sencilla entre los distintos contenidos y permite gestionar fácilmente el orden de reproducción.\n"
        },
        {"de", 
            "Diese Funktion ist nur im Einzelansicht-Modus verfügbar.\n"
            "Die Playlist ermöglicht es, mehrere Medien innerhalb derselben Wiedergabeliste zu organisieren und nacheinander abzuspielen.\n"
            "So bietet sie eine einfache Navigation zwischen den verschiedenen Inhalten und ermöglicht eine leichte Verwaltung der Wiedergabereihenfolge.\n"
        },
        {"it", 
            "Questa funzionalità è disponibile solo in modalità vista singola.\n"
            "La playlist consente di organizzare più contenuti multimediali all'interno della stessa lista di riproduzione e di riprodurli in successione.\n"
            "Offre così una navigazione semplice tra i diversi contenuti e permette di gestire facilmente l'ordine di riproduzione.\n"
        },
        {"pt", 
            "Este recurso está disponível apenas no modo de visualização única.\n"
            "A playlist permite organizar várias mídias dentro de uma mesma lista de reprodução e reproduzi-las sucessivamente.\n"
            "Assim, ela oferece uma navegação simples entre os diferentes conteúdos e permite gerenciar facilmente a ordem de reprodução.\n"
        }
    });

    widget->addImage("navpanel_playlist");

    return widget;
}

QWidget* PlaylistContent::managePlaylist(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Plusieurs fonctionnalités permettent d'ajouter, de supprimer et de réorganiser les médias au sein de la liste de lecture.\n"
            "Pour ajouter un média à la playlist, cliquer sur l'icône d'ajout puis sélectionner le fichier souhaité, ou effectuer un glisser-déposer.\n"
            "Pour supprimer un média, cliquer sur l'icône représentée par une poubelle.\n"
            "Pour réorganiser l'ordre des médias, les faire glisser à l'endroit souhaité à l'aide de la souris.\n"
            "Deux icônes sont dédiées à la sauvegarde et à l'importation d'une playlist au format XSPF.\n"
            "Des boutons de lecture en mode boucle, en mode aléatoire et de mise en lecture automatique se situent en bas de la playlist.\n"
        },
        {"en", 
            "Several features let you add, remove, and reorder media within the playlist.\n"
            "To add media to the playlist, click the add icon and select the desired file, or use drag-and-drop.\n"
            "To remove media, click the icon shown as a trash can.\n"
            "To reorder the media, drag them to the desired position using the mouse.\n"
            "Two icons are dedicated to saving and importing a playlist in XSPF format.\n"
            "Loop mode, shuffle mode, and autoplay buttons are located at the bottom of the playlist.\n"
        },
        {"es", 
            "Varias funciones permiten añadir, eliminar y reorganizar los medios dentro de la lista de reproducción.\n"
            "Para añadir un medio a la playlist, haga clic en el icono de añadir y seleccione el archivo deseado, o utilice arrastrar y soltar.\n"
            "Para eliminar un medio, haga clic en el icono representado por una papelera.\n"
            "Para reorganizar el orden de los medios, arrástrelos hasta la posición deseada con el ratón.\n"
            "Dos iconos están dedicados al guardado y a la importación de una playlist en formato XSPF.\n"
            "Los botones de modo bucle, modo aleatorio y reproducción automática se encuentran en la parte inferior de la playlist.\n"
        },
        {"de", 
            "Mehrere Funktionen ermöglichen das Hinzufügen, Entfernen und Neuordnen von Medien innerhalb der Wiedergabeliste.\n"
            "Um ein Medium zur Playlist hinzuzufügen, klicken Sie auf das Hinzufügen-Symbol und wählen Sie die gewünschte Datei aus, oder nutzen Sie Drag-and-Drop.\n"
            "Um ein Medium zu entfernen, klicken Sie auf das Symbol in Form eines Papierkorbs.\n"
            "Um die Reihenfolge der Medien zu ändern, ziehen Sie sie mit der Maus an die gewünschte Position.\n"
            "Zwei Symbole dienen dem Speichern und Importieren einer Playlist im XSPF-Format.\n"
            "Die Schaltflächen für Loop-Modus, Zufallsmodus und automatische Wiedergabe befinden sich unten in der Playlist.\n"
        },
        {"it", 
            "Diverse funzionalità consentono di aggiungere, rimuovere e riordinare i contenuti multimediali all'interno della playlist.\n"
            "Per aggiungere un contenuto alla playlist, fare clic sull'icona di aggiunta e selezionare il file desiderato, oppure trascinarlo e rilasciarlo.\n"
            "Per rimuovere un contenuto, fare clic sull'icona a forma di cestino.\n"
            "Per riordinare i contenuti, trascinarli nella posizione desiderata con il mouse.\n"
            "Due icone sono dedicate al salvataggio e all'importazione di una playlist in formato XSPF.\n"
            "I pulsanti per la modalità loop, la modalità casuale e la riproduzione automatica si trovano nella parte inferiore della playlist.\n"
        },
        {"pt", 
            "Vários recursos permitem adicionar, remover e reorganizar as mídias dentro da lista de reprodução.\n"
            "Para adicionar uma mídia à playlist, clique no ícone de adicionar e selecione o arquivo desejado, ou use arrastar e soltar.\n"
            "Para remover uma mídia, clique no ícone representado por uma lixeira.\n"
            "Para reorganizar a ordem das mídias, arraste-as até a posição desejada com o mouse.\n"
            "Dois ícones são dedicados ao salvamento e à importação de uma playlist no formato XSPF.\n"
            "Os botões de modo de repetição, modo aleatório e reprodução automática ficam na parte inferior da playlist.\n"
        }
    });

    widget->addImage("playlist_open_dialog");
    widget->addImages({"playlist_remove", "playlist_sort"});

    return widget;
}