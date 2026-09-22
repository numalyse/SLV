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
            "Multi-view mode allows you to view up to 4 media items at the same time within the same interface.\n"
            "An additional navigation bar is added to the interface, allowing all media items to be controlled simultaneously.\n"
            "It makes it possible to view different content in parallel, making it easier to consult or compare them simultaneously. Each view can be used independently while retaining access to the main playback and navigation tools. \n"
            "The display can be adapted to suit your needs in order to organize the media clearly and make full use of the available space.\n"
        },
        {"es", 
            "El modo de vista múltiple permite visualizar hasta 4 contenidos multimedia a la vez dentro de la misma interfaz.\n"
            "Se añade una barra de navegación adicional a la interfaz que permite controlar todos los contenidos multimedia simultáneamente.\n"
            "Ofrece la posibilidad de visualizar diferentes contenidos en paralelo, facilitando así su consulta o comparación simultánea. Cada vista puede utilizarse de forma independiente, manteniendo el acceso a las principales herramientas de reproducción y navegación. \n"
            "La visualización puede adaptarse según las necesidades para organizar los contenidos multimedia de forma clara y aprovechar al máximo el espacio disponible.\n"
        },
        {"de", 
            "Der Mehrfachansichtsmodus ermöglicht die gleichzeitige Anzeige von bis zu 4 Medien innerhalb derselben Benutzeroberfläche.\n"
            "Die Benutzeroberfläche wird um eine zusätzliche Navigationsleiste erweitert, mit der alle Medien gleichzeitig gesteuert werden können.\n"
            "Dadurch können verschiedene Inhalte parallel angezeigt werden, was deren gleichzeitige Betrachtung oder den Vergleich erleichtert. Jede Ansicht kann unabhängig verwendet werden, wobei der Zugriff auf die wichtigsten Wiedergabe- und Navigationswerkzeuge erhalten bleibt. \n"
            "Die Anzeige kann je nach Bedarf angepasst werden, um die Medien übersichtlich anzuordnen und den verfügbaren Platz optimal zu nutzen.\n"
        },
        {"it", 
            "La modalità multi-vista consente di visualizzare fino a 4 contenuti multimediali contemporaneamente all'interno della stessa interfaccia.\n"
            "All'interfaccia viene aggiunta una barra di navigazione aggiuntiva che permette di controllare tutti i contenuti multimediali simultaneamente.\n"
            "Offre la possibilità di visualizzare diversi contenuti in parallelo, facilitandone così la consultazione o il confronto simultaneo. Ogni vista può essere utilizzata indipendentemente, mantenendo l'accesso ai principali strumenti di riproduzione e navigazione. \n"
            "La visualizzazione può essere adattata in base alle esigenze per organizzare i contenuti multimediali in modo chiaro e sfruttare al meglio lo spazio disponibile.\n"
        },
        {"pt", 
            "O modo de visualização múltipla permite visualizar até 4 conteúdos multimédia em simultâneo na mesma interface.\n"
            "É adicionada à interface uma barra de navegação adicional que permite controlar todos os conteúdos multimédia em simultâneo.\n"
            "Permite visualizar diferentes conteúdos em paralelo, facilitando a sua consulta ou comparação simultânea. Cada vista pode ser utilizada de forma independente, mantendo o acesso às principais ferramentas de reprodução e navegação. \n"
            "A apresentação pode ser adaptada de acordo com as necessidades, de modo a organizar os conteúdos multimédia de forma clara e tirar o máximo partido do espaço disponível.\n"
        }
    });

    widget->addImage("multiview_display");

    widget->addImages({"player2", "player2", "player3"});

    widget->addTextFromLangQMAP({
        {"fr", 
            "Deux méthodes permettent d'afficher plusieurs médias au sein du Numalyse Player. \n"
            "Leur différence repose sur la façon dont elles agissent.\n"
            "Le premier permet d'ajouter ou supprimer plusieurs lecteurs à la fois tandis que le second duplique un lecteur existant et son contenu.\n"
        },
        {"en", 
            "Two methods are available for displaying multiple media items in the Numalyse Player. \n"
            "The difference between them lies in how they operate.\n"
            "The first allows multiple players to be added or removed at once, while the second duplicates an existing player and its content.\n"
        },
        {"es", 
            "Hay dos métodos disponibles para mostrar varios contenidos multimedia en Numalyse Player. \n"
            "La diferencia entre ellos radica en la forma en que funcionan.\n"
            "El primero permite añadir o eliminar varios reproductores a la vez, mientras que el segundo duplica un reproductor existente y su contenido.\n"
        },
        {"de", 
            "Im Numalyse Player stehen zwei Methoden zur Verfügung, um mehrere Medien anzuzeigen. \n"
            "Der Unterschied zwischen ihnen liegt in ihrer Funktionsweise.\n"
            "Mit der ersten Methode können mehrere Player gleichzeitig hinzugefügt oder entfernt werden, während die zweite einen vorhandenen Player und dessen Inhalt dupliziert.\n"
        },
        {"it", 
            "Sono disponibili due metodi per visualizzare più contenuti multimediali all'interno di Numalyse Player. \n"
            "La differenza tra i due risiede nel modo in cui agiscono.\n"
            "Il primo consente di aggiungere o rimuovere più lettori contemporaneamente, mentre il secondo duplica un lettore esistente e il relativo contenuto.\n"
        },
        {"pt", 
            "Estão disponíveis dois métodos para apresentar vários conteúdos multimédia no Numalyse Player. \n"
            "A diferença entre eles reside na forma como funcionam.\n"
            "O primeiro permite adicionar ou remover vários leitores em simultâneo, enquanto o segundo duplica um leitor existente e o respetivo conteúdo.\n"
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
        {"en", {"Via the display mode icon",
            "To quickly access multi-view mode, hover over the player layout icon at the top right of the interface.\n"
            "Move the mouse and click the icon corresponding to the desired layout. "
            "(In 2- or 3-view mode, you can choose the desired layout.)\n"
            "One or more players will be added or removed if the number of views is changed.\n"}
        },
        {"es", {"Mediante el icono del modo de visualización",
            "Para acceder rápidamente al modo de vista múltiple, coloque el cursor sobre el icono de disposición de los reproductores situado en la parte superior derecha de la interfaz.\n"
            "Desplace el ratón y haga clic en el icono correspondiente a la disposición deseada. "
            "(En el modo de 2 o 3 vistas, es posible elegir la disposición.)\n"
            "Se añadirán o eliminarán uno o varios reproductores si se modifica el número de vistas.\n"}
        },
        {"de", {"Über das Symbol für den Anzeigemodus",
            "Um schnell auf den Mehrfachansichtsmodus zuzugreifen, bewegen Sie den Mauszeiger oben rechts in der Benutzeroberfläche über das Symbol für die Anordnung der Player.\n"
            "Bewegen Sie die Maus und klicken Sie auf das Symbol für die gewünschte Anordnung. "
            "(Im 2- oder 3-Ansichten-Modus kann die gewünschte Anordnung ausgewählt werden.)\n"
            "Wenn die Anzahl der Ansichten geändert wird, werden ein oder mehrere Player hinzugefügt oder entfernt.\n"}
        },
        {"it", {"Tramite l'icona della modalità di visualizzazione",
            "Per accedere rapidamente alla modalità multi-vista, nella parte superiore destra dell'interfaccia, posizionare il cursore sull'icona di disposizione dei lettori.\n"
            "Spostare il mouse e fare clic sull'icona corrispondente alla disposizione desiderata. "
            "(In modalità a 2 o 3 viste, è possibile scegliere la disposizione.)\n"
            "Uno o più lettori verranno aggiunti o rimossi se il numero di viste viene modificato.\n"}
        },
        {"pt", {"Através do ícone do modo de visualização",
            "Para aceder rapidamente ao modo de visualização múltipla, na parte superior direita da interface, passe o cursor sobre o ícone de disposição dos leitores.\n"
            "Mova o rato e clique no ícone correspondente à disposição pretendida. "
            "(No modo de 2 ou 3 vistas, é possível escolher a disposição.)\n"
            "Um ou mais leitores serão adicionados ou removidos se o número de vistas for alterado.\n"}
        }
    });

    widget->addImages({"views_all"});
    widget->addImages({"views_2", "views_3"});

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
            "Click the duplicate icon on a player. The media will be duplicated into a new view while keeping its settings.\n"
            "This method lets you view the same media in multiple players and control them independently.\n"
            "This way, you can compare different parts of the media or apply distinct settings to each view.\n"
            "This feature can be particularly useful for detailed analysis of the same media.\n\n"
            "Note: The duplicated player uses a default layout if there are two or three of them."
            "\n"}
        },
        {"es", {"",
            "Haga clic en el icono de duplicación de un reproductor. El medio se duplicará así en una nueva vista conservando sus ajustes.\n"
            "Este método permite visualizar el mismo medio en varios reproductores y controlarlos de forma independiente.\n"
            "De este modo, es posible comparar diferentes partes del medio o aplicar ajustes distintos a cada vista.\n"
            "Esta función puede resultar especialmente útil para el análisis detallado de un mismo medio.\n\n"
            "Atención: El reproductor duplicado tiene una disposición predeterminada si hay dos o tres."
            "\n"}
        },
        {"de", {"",
            "Klicken Sie auf das Duplizieren-Symbol eines Players. Das Medium wird so in einer neuen Ansicht dupliziert, wobei die Einstellungen beibehalten werden.\n"
            "Mit dieser Methode können Sie dasselbe Medium in mehreren Playern anzeigen und diese unabhängig voneinander steuern.\n"
            "So lassen sich unterschiedliche Teile des Mediums vergleichen oder für jede Ansicht individuelle Einstellungen anwenden.\n"
            "Diese Funktion kann besonders nützlich für die detaillierte Analyse desselben Mediums sein.\n\n"
            "Hinweis: Der duplizierte Player verwendet ein Standardlayout, wenn es zwei oder drei davon gibt."
            "\n"}
        },
        {"it", {"",
            "Fare clic sull'icona di duplicazione di un lettore. Il contenuto multimediale verrà così duplicato in una nuova vista mantenendo le sue impostazioni.\n"
            "Questo metodo consente di visualizzare lo stesso contenuto multimediale in più lettori e di controllarli in modo indipendente.\n"
            "In questo modo è possibile confrontare diverse parti del contenuto o applicare impostazioni distinte a ciascuna vista.\n"
            "Questa funzione può essere particolarmente utile per l'analisi dettagliata dello stesso contenuto multimediale.\n\n"
            "Attenzione: il lettore duplicato ha una disposizione predefinita se ce ne sono due o tre."
            "\n"}
        },
        {"pt", {"",
            "Clique no ícone de duplicação de um leitor. A mídia será assim duplicada em uma nova visualização, mantendo suas configurações.\n"
            "Este método permite visualizar a mesma mídia em vários leitores e controlá-los de forma independente.\n"
            "Assim, é possível comparar diferentes partes da mídia ou aplicar ajustes distintos a cada visualização.\n"
            "Este recurso pode ser particularmente útil para a análise detalhada de uma mesma mídia.\n\n"
            "Atenção: o leitor duplicado tem uma disposição padrão se houver dois ou três."
            "\n"}
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
        {"en", {"Individual navigation bar",
            "A simplified playback bar lets you manage each player independently. It provides the essential features to control the playback of each media.\n"
            "\n"}
        },
        {"es", {"Barra de navegación individual",
            "Una barra de reproducción simplificada permite gestionar cada reproductor de forma independiente. Ofrece las funciones esenciales para controlar la reproducción de cada medio.\n"
            "\n"}
        },
        {"de", {"Individuelle Navigationsleiste",
            "Eine vereinfachte Wiedergabeleiste ermöglicht die unabhängige Verwaltung jedes Players. Sie bietet die wesentlichen Funktionen zur Steuerung der Wiedergabe jedes Mediums.\n"
            "\n"}
        },
        {"it", {"Barra di navigazione individuale",
            "Una barra di riproduzione semplificata consente di gestire ciascun lettore in modo indipendente. Offre le funzionalità essenziali per controllare la riproduzione di ogni contenuto multimediale.\n"
            "\n"}
        },
        {"pt", {"Barra de navegação individual",
            "Uma barra de reprodução simplificada permite gerenciar cada leitor de forma independente. Ela oferece as funcionalidades essenciais para controlar a reprodução de cada mídia.\n"
            "\n"}
        }
    });

    widget->addImage("");

    widget->addQMAPTexts({
        {"fr", {"Barre de navigation commune",
            "Une barre de navigation commune permet de gérer tous les lecteurs simultanément.\n"
            "\n"}
        },
        {"en", {"Common navigation bar",
            "A common navigation bar lets you manage all players simultaneously.\n"
            "\n"}
        },
        {"es", {"Barra de navegación común",
            "Una barra de navegación común permite gestionar todos los reproductores simultáneamente.\n"
            "\n"}
        },
        {"de", {"Gemeinsame Navigationsleiste",
            "Eine gemeinsame Navigationsleiste ermöglicht die gleichzeitige Verwaltung aller Player.\n"
            "\n"}
        },
        {"it", {"Barra di navigazione comune",
            "Una barra di navigazione comune consente di gestire tutti i lettori simultaneamente.\n"
            "\n"}
        },
        {"pt", {"Barra de navegação comum",
            "Uma barra de navegação comum permite gerenciar todos os leitores simultaneamente.\n"
            "\n"}
        }
    });

    widget->addImage("");

    widget->addQMAPTexts({
        {"fr", {"Capture d'écran multiple",
            "Le bouton de capture de la barre commune permet de prendre une capture d'écran de chaque média et les combine entre elles en une seule image en gardant la disposition des lecteurs.\n"
            "Les captures d'écrans sont enregistrées dans le dossier choisi par l'utilisateur dans les paramètres de préférence. (NumalysePlayer_Content par défaut)\n"}
        },
        {"en", {"Multiple screenshot",
            "The capture button on the common bar takes a screenshot of each media and combines them into a single image, keeping the players' layout.\n"
            "The screenshots are saved in the folder chosen by the user in the preference settings. (NumalysePlayer_Content by default)\n"}
        },
        {"es", {"Captura de pantalla múltiple",
            "El botón de captura de la barra común toma una captura de pantalla de cada medio y las combina en una sola imagen, conservando la disposición de los reproductores.\n"
            "Las capturas de pantalla se guardan en la carpeta elegida por el usuario en los ajustes de preferencias. (NumalysePlayer_Content por defecto)\n"}
        },
        {"de", {"Mehrfacher Screenshot",
            "Die Aufnahme-Schaltfläche der gemeinsamen Leiste erstellt einen Screenshot jedes Mediums und kombiniert sie zu einem einzigen Bild, wobei die Anordnung der Player beibehalten wird.\n"
            "Die Screenshots werden in dem vom Benutzer in den Einstellungen ausgewählten Ordner gespeichert. (standardmäßig NumalysePlayer_Content)\n"}
        },
        {"it", {"Cattura schermo multipla",
            "Il pulsante di cattura della barra comune scatta uno screenshot di ciascun contenuto multimediale e li combina in un'unica immagine, mantenendo la disposizione dei lettori.\n"
            "Gli screenshot vengono salvati nella cartella scelta dall'utente nelle impostazioni delle preferenze. (NumalysePlayer_Content per impostazione predefinita)\n"}
        },
        {"pt", {"Captura de tela múltipla",
            "O botão de captura da barra comum tira uma captura de tela de cada mídia e as combina em uma única imagem, mantendo a disposição dos leitores.\n"
            "As capturas de tela são salvas na pasta escolhida pelo usuário nas configurações de preferências. (NumalysePlayer_Content por padrão)\n"}
        }
    });

    widget->addImage("multiview_screenshot");
    widget->addImage("multiview_screenshot_dialog");
    
    return widget;
}

