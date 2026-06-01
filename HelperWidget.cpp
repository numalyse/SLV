#include "HelperWidget.h"

#include "PrefManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QString>

HelperWidget::HelperWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(PrefManager::instance().getText("help_dialog_title"));
    setFixedWidth(500);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 40, 20, 40);

    QLabel *text = new QLabel(PrefManager::instance().getText("help_dialog_wip"), this);
    text->setAlignment(Qt::AlignCenter);
    text->setStyleSheet("font-style: italic;");
    layout->addWidget(text);

    layout->addSpacing(40);

    QLabel *report = new QLabel(PrefManager::instance().getText("help_dialog_report"), this);
    report->setAlignment(Qt::AlignCenter);
    layout->addWidget(report);

    QString mails = 
        QStringLiteral("<a href=\"christina.maurin@umpv.fr\">Christina.Maurin@umpv.fr</a>") + 
        QStringLiteral("<br>") +
        QStringLiteral("<a href=\"killian.viguier@umpv.fr\">Killian.Viguier@umpv.fr</a>");

    QLabel *label_mails = new QLabel(mails, this);
    label_mails->setAlignment(Qt::AlignCenter);
    label_mails->setTextFormat(Qt::RichText);
    label_mails->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label_mails->setOpenExternalLinks(true);
    label_mails->setStyleSheet("a { text-decoration: none; }");
    layout->addWidget(label_mails);
    
    

    // // Bouton fermer
    // QPushButton *closeButton = new QPushButton(PrefManager::instance().getText("close"), this);
    // layout->addWidget(closeButton, 0, Qt::AlignRight);

    // connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);
}

HelperWidget::~HelperWidget()
{
}
