#include "GeneralContent.h"

#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>

GeneralContent::GeneralContent(QWidget* parent)
    : CategoryBase("help_menu_general_category", parent)
{
    QString introductionName = "help_menu_introduction_label";
    QString reportIssueName = "help_menu_report_issues_label";

    addSubcategory(
        introductionName,
        introduction(introductionName)
    );

    addSubcategory(
        reportIssueName,
        reportIssue(reportIssueName)
    );

}

QWidget* GeneralContent::introduction(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    QLabel *appIcon = new QLabel();
    appIcon->setPixmap(QPixmap(":/logo/numalyse_player_logo")
            .scaled(100, 100, 
                    Qt::KeepAspectRatio, 
                    Qt::SmoothTransformation));
    appIcon->setAlignment(Qt::AlignCenter);
    widget->addContent(appIcon);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Numalyse Player est un logiciel gratuit, open source et multi-plateforme (Windows et MacOS) développé dans le cadre du projet ANR Numalyse (2024-2028).\n"
            "L'application permet d'ouvrir et de lire différents types de contenus multimédias depuis une seule interface. Elle prend en charge différents types de formats de fichiers permettant ainsi un accès facilité à vos documents audio, vidéo et image."
        },
        {"en", 
            "Numalyse Player is a free, open source and cross-platform (Window and MacOS) software developed under the ANR Numalyse project (2024-2028).\n"
            "The application allows you to open and play various types of multimedia content from a single interface. It supports a range of file formats, providing easy access to your audio, video, and image files."
        },
        {"es", 
            "Numalyse Player es un software gratuito, de código abierto y multiplataforma (Windows y MacOS) desarrollado en el marco del proyecto ANR Numalyse (2024-2028).\n"
            "La aplicación permite abrir y reproducir diferentes tipos de contenidos multimedia desde una única interfaz. Es compatible con varios formatos de archivo, facilitando así el acceso a sus documentos de audio, vídeo e imagen."
        },
        {"de", 
            "Numalyse Player ist eine kostenlose, Open-Source- und plattformübergreifende Software (Windows und MacOS), die im Rahmen des ANR Numalyse-Projekts (2024-2028) entwickelt wurde.\n"
            "Die Anwendung ermöglicht das Öffnen und Wiedergeben verschiedener Arten von Multimedia-Inhalten über eine einzige Benutzeroberfläche. Sie unterstützt zahlreiche Dateiformate und erleichtert so den Zugriff auf Ihre Audio-, Video- und Bilddateien."
        },
        {"it", 
            "Numalyse Player è un software gratuito, open source e multipiattaforma (Windows e MacOS) sviluppato nell'ambito del progetto ANR Numalyse (2024-2028).\n"
            "L'applicazione consente di aprire e riprodurre diversi tipi di contenuti multimediali da un'unica interfaccia. Supporta vari formati di file, facilitando così l'accesso ai vostri documenti audio, video e immagini."
        },
        {"pt", 
            "Numalyse Player é um software gratuito, open source e multiplataforma (Windows e MacOS) desenvolvido no âmbito do projeto ANR Numalyse (2024-2028).\n"
            "A aplicação permite abrir e reproduzir diferentes tipos de conteúdos multimédia a partir de uma única interface. Suporta vários formatos de ficheiros, facilitando o acesso aos seus documentos de áudio, vídeo e imagem."
        }
    });

    return widget;
}

QWidget* GeneralContent::reportIssue(const QString& subcategoryName)
{
    auto* widget = new ContentBase(this, categoryName(), subcategoryName);

    widget->addTextFromLangQMAP({
        {"fr", 
            "Numalyse Player est un logiciel en cours de développement.\n"
            "Nous vous invitons à nous contacter à l'adresse ci-dessous en cas de problèmes, afin de signaler un bug ou également en cas de difficultés de prise en main avec le logiciel."
        },
        {"en", 
            "Numalyse Player is currently under development.\n"
            "We invite you to contact us at the address below in case of any issues, to report a bug, or if you encounter any difficulties getting started with the software."
        },
        {"es", 
            "Numalyse Playeres un software que se encuentra actualmente en desarrollo.\n"
            "Le invitamos a ponerse en contacto con nosotros en la dirección indicada a continuación en caso de problemas, para informar de un error o si tiene dificultades para familiarizarse con el uso del software."
        },
        {"de", 
            "Numalyse Player ist eine Software, die sich derzeit in der Entwicklung befindet."
            "Wir laden Sie ein, uns unter der unten angegebenen Adresse zu kontaktieren, wenn Probleme auftreten, um einen Fehler zu melden oder wenn Sie Schwierigkeiten bei der Nutzung der Software haben."
        },
        {"it", 
            "Numalyse Player è un software attualmente in fase di sviluppo.\n"
            "Vi invitiamo a contattarci all'indirizzo indicato di seguito in caso di problemi, per segnalare un bug o in caso di difficoltà nell'utilizzo del software."
        },
        {"pt", 
            "Numalyse Playeré um software atualmente em desenvolvimento.\n"
            "Convidamo-lo a contactar-nos através do endereço abaixo caso encontre algum problema, para comunicar um erro ou em caso de dificuldades na utilização do software."
        }
    });

    QStringList mailinglist = QStringList{"numalyse.slv@gmail.com", "christina.maurin@umpv.fr"};

    widget->addMails(mailinglist);

    return widget;
}