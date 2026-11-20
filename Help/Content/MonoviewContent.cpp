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
    QString openMediaTitle = "help_menu_open_file_label";
    QString controlMediaTitle = "help_menu_control_media_label";
    QString paramMediaTitle = "help_menu_param_media_label";
    QString zoomMediaTitle = "help_menu_zoom_media_label";
    QString altMediaTitle = "help_menu_alt_media_label";
    QString drawingModeTitle = "help_menu_drawingmode_label";
    QString compoRulesTitle = "help_menu_comporules_label";
    QString screenshotTitle = "help_menu_screenshot_label";
    QString recordingTitle = "help_menu_recording_label";
    QString extractSequenceTitle = "help_menu_extract_sequence_label";

    addSubcategory(
        presentationTitle,
        presentation(presentationTitle)
    );

    addSubcategory(
        openMediaTitle,
        openMedia(openMediaTitle)
    );

    addSubcategory(
        controlMediaTitle,
        controlMedia(controlMediaTitle)
    );

    addSubcategory(
        paramMediaTitle,
        paramMedia(paramMediaTitle)
    );

    addSubcategory(
        zoomMediaTitle,
        zoomMedia(zoomMediaTitle)
    );

    addSubcategory(
        altMediaTitle,
        altMedia(altMediaTitle)
    );

    addSubcategory(
        drawingModeTitle,
        drawingMode(drawingModeTitle)
    );

    addSubcategory(
        compoRulesTitle,
        compoRules(compoRulesTitle)
    );

    addSubcategory(
        screenshotTitle,
        screenshot(screenshotTitle)
    );

    addSubcategory(
        recordingTitle,
        recording(recordingTitle)
    );

    addSubcategory(
        extractSequenceTitle,
        extractSequence(extractSequenceTitle)
    );

    
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
            "Il permet de visualiser un seul média à la fois et de disposer des outils nécessaires pour travailler et interagir avec lui.\n"
            "\n"
            "Sa barre d’outils, semblable à celle d’autres logiciels de lecture vidéo courants, peut être étendue afin d’accéder à des fonctionnalités supplémentaires, plus techniques.\n"
            "\n"
            "Parmi celles-ci, une timeline détaillée permet de visualiser et de naviguer plus précisément dans le média : elle donne un repérage fin de la position de lecture et facilite le déplacement vers un moment précis.\n"
            
        },
        {"en", 
            "Single-view mode is the default display mode of the media player.\n"
            
        },
        {"es", 
            "El modo de vista única es el modo de visualización predeterminado del reproductor multimedia.\n"
            
        },
        {"de", 
            "Der Einzelansichtsmodus ist der standardmäßige Anzeigemodus des Medienplayers.\n"
            
        },
        {"it", 
            "La modalità a visualizzazione singola è la modalità di visualizzazione predefinita del lettore multimediale.\n"
            
        },
        {"pt", 
            "O modo de visualização única é o modo de apresentação predefinido do leitor multimédia.\n"
            
        }
    });

    return widget;
}

QWidget* MonoviewContent::openMedia(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    //widget->addTextFromLangJSON("help_menu_open_files_content_1");

    widget->addTextFromLangQMAP({
        {"fr", 
            "Pour ouvrir un média, plusieurs méthodes sont disponibles.\n"
        },
        {"en", 
            "There are several ways to open a media item.\n"
        },
        {"es", 
            "Hay varios métodos disponibles para abrir un contenido multimedia.\n"
        },
        {"de", 
            "Zum Öffnen eines Mediums stehen mehrere Methoden zur Verfügung.\n"
        },
        {"it", 
            "Sono disponibili diversi metodi per aprire un contenuto multimediale.\n"
        },
        {"pt", 
            "Estão disponíveis vários métodos para abrir um conteúdo multimédia.\n"
        }
    });

    //widget->addTextFromLangQMAP(title);

    widget->addQMAPTexts({
        {"fr", {"Depuis le menu Fichier",
            "Cliquer sur la barre de menu « Fichier », sélectionner « Ouvrir un média… » pour parcourir les fichiers disponibles sur l'ordinateur. Sélectionner ensuite le média souhaité pour l'ouvrir dans l'application.\n"}
        },

        {"en", {"From the File menu",
            "Click the « File » menu, select « Open a media… » to browse the files available on the computer. Then select the desired media to open it in the application.\n"}
        },

        {"es", {"Desde el menú Archivo",
            "Haga clic en el menú « Archivo » y seleccione « Abrir un contenido multimedia… » para explorar los archivos disponibles en el ordenador. A continuación, seleccione el contenido multimedia deseado para abrirlo en la aplicación.\n"}
        },

        {"de", {"Über das Menü Datei",
            "Klicken Sie auf das Menü « Datei » und wählen Sie « Medium öffnen… », um die auf dem Computer verfügbaren Dateien zu durchsuchen. Wählen Sie anschließend das gewünschte Medium aus, um es in der Anwendung zu öffnen.\n"}
        },

        {"it", {"Dal menu File",
            "Fare clic sul menu « File » e selezionare « Apri un contenuto multimediale… » per esplorare i file disponibili sul computer. Selezionare quindi il contenuto multimediale desiderato per aprirlo nell'applicazione.\n"}
        },

        {"pt", {"A partir do menu Ficheiro",
            "Clique no menu « Ficheiro » e selecione « Abrir um conteúdo multimédia… » para procurar os ficheiros disponíveis no computador. Em seguida, selecione o conteúdo multimédia pretendido para o abrir na aplicação.\n"}
        }
    });

    widget->addImage("file_open_dialog");

    widget->addQMAPTexts({
        {"fr", {"Par glisser-déposer",
            "Un média peut également être ouvert directement par glisser-déposer. Faire glisser le fichier depuis son emplacement sur l'ordinateur, puis le déposer dans la fenêtre de l'application. Le média est alors automatiquement chargé.\n"}
        },

        {"en", {"By drag and drop",
            "A media item can also be opened directly by drag and drop. Drag the file from its location on the computer and drop it into the application window. The media is then loaded automatically.\n"}
        },

        {"es", {"Mediante arrastrar y soltar",
            "También se puede abrir un contenido multimedia directamente mediante arrastrar y soltar. Arrastre el archivo desde su ubicación en el ordenador y suéltelo en la ventana de la aplicación. El contenido multimedia se cargará automáticamente.\n"}
        },

        {"de", {"Per Drag & Drop",
            "Ein Medium kann auch direkt per Drag & Drop geöffnet werden. Ziehen Sie die Datei von ihrem Speicherort auf dem Computer in das Anwendungsfenster und legen Sie sie dort ab. Das Medium wird anschließend automatisch geladen.\n"}
        },

        {"it", {"Tramite trascinamento",
            "È inoltre possibile aprire direttamente un contenuto multimediale tramite trascinamento. Trascinare il file dalla sua posizione sul computer e rilasciarlo nella finestra dell'applicazione. Il contenuto multimediale verrà quindi caricato automaticamente.\n"}
        },

        {"pt", {"Por arrastar e largar",
            "Também é possível abrir diretamente um conteúdo multimédia através da função de arrastar e largar. Arraste o ficheiro a partir da sua localização no computador e largue-o na janela da aplicação. O conteúdo multimédia será então carregado automaticamente.\n"}
        }
    });

    widget->addImage("dragdrop_file");

    widget->addQMAPTexts({
        {"fr", {"Avec le bouton Lecture",
            "Lorsqu'aucun média n'est déjà ouvert, le bouton Lecture permet également d'ouvrir un fichier. Cliquer sur ce bouton pour afficher le sélecteur de fichiers, puis choisir le média à ouvrir.\n"}
        },

        {"en", {"With the Play button",
            "When no media is already open, the Play button can also be used to open a file. Click this button to display the file selector, then choose the media to open.\n"}
        },

        {"es", {"Con el botón Reproducir",
            "Cuando no hay ningún contenido multimedia abierto, el botón Reproducir también permite abrir un archivo. Haga clic en este botón para mostrar el selector de archivos y, a continuación, seleccione el contenido multimedia que desea abrir.\n"}
        },

        {"de", {"Mit der Wiedergabetaste",
            "Wenn noch kein Medium geöffnet ist, kann über die Wiedergabetaste ebenfalls eine Datei geöffnet werden. Klicken Sie auf diese Schaltfläche, um den Dateiauswahldialog anzuzeigen, und wählen Sie anschließend das gewünschte Medium aus.\n"}
        },

        {"it", {"Con il pulsante Riproduci",
            "Quando non è già aperto alcun contenuto multimediale, il pulsante Riproduci consente anche di aprire un file. Fare clic su questo pulsante per visualizzare il selettore di file, quindi scegliere il contenuto multimediale da aprire.\n"}
        },

        {"pt", {"Com o botão Reproduzir",
            "Quando não está aberto nenhum conteúdo multimédia, o botão Reproduzir também permite abrir um ficheiro. Clique neste botão para apresentar o seletor de ficheiros e, em seguida, selecione o conteúdo multimédia a abrir.\n"}
        }
    });

    widget->addImage("player");

    return widget;
}

QWidget* MonoviewContent::controlMedia(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);


    return widget;
}

QWidget* MonoviewContent::extractSequence(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addQMAPTexts({
        {"fr", {"Extraire une séquence",
            "Une fenêtre s'ouvre pour configurer l'extraction.\n"
            "Par défaut, le timecode de début est celui correspondant au curseur temporel. Si possible, +10 secondes sont ajoutées par défaut au timecode de fin.\n"}
        },
        {"en", {"Extract a sequence",
            "A window opens to configure the extraction.\n"
            "By default, the start timecode is the one corresponding to the time cursor. If possible, +10 seconds are added by default to the end timecode.\n"}
        },
        {"es", {"Extraer una secuencia",
            "Se abre una ventana para configurar la extracción.\n"
            "Por defecto, el timecode de inicio es el que corresponde al cursor temporal. Si es posible, se añaden por defecto +10 segundos al timecode de fin.\n"}
        },
        {"de", {"Eine Sequenz extrahieren",
            "Ein Fenster öffnet sich, um die Extraktion zu konfigurieren.\n"
            "Standardmäßig entspricht der Start-Timecode der Position des Zeitcursors. Falls möglich, werden standardmäßig +10 Sekunden zum End-Timecode hinzugefügt.\n"}
        },
        {"it", {"Estrarre una sequenza",
            "Si apre una finestra per configurare l'estrazione.\n"
            "Per impostazione predefinita, il timecode di inizio è quello corrispondente al cursore temporale. Se possibile, vengono aggiunti per impostazione predefinita +10 secondi al timecode di fine.\n"}
        },
        {"pt", {"Extrair uma sequência",
            "Uma janela é aberta para configurar a extração.\n"
            "Por padrão, o timecode de início é aquele correspondente ao cursor temporal. Se possível, +10 segundos são adicionados por padrão ao timecode de fim.\n"}
        }
    });

    widget->addImages({"extract_sequence", "extract_sequence_options"});


    return widget;
}

// QWidget* MonoviewContent::formats(const QString& subcategoryName)
// {
//     auto* widget = new ContentBase(this, categoryName(), subcategoryName);


//     return widget;
// }