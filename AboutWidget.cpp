#include "AboutWidget.h"
#include "PrefManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutWidget::AboutWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(PrefManager::instance().getText("about_dialog_title"));
    setFixedWidth(500);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 40, 20, 40);

    QLabel *appIcon = new QLabel();
    appIcon->setPixmap(QPixmap(":/logo/numalyse_logo_white").scaled(100, 100));
    appIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(appIcon);

    // Titre / nom de l'app
    QLabel *title = new QLabel("<h2>Numalyse Player</h2>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Version
    QLabel *version = new QLabel(
                                "<i>V1.0-alpha (" 
                                + PrefManager::instance().getText("about_dialog_last_update") 
                                + "2026-05-05)</i>",
                                this
    );
    version->setAlignment(Qt::AlignCenter);
    layout->addWidget(version);

    layout->addSpacing(40);

    // Description
    QLabel *desc = new QLabel(
        PrefManager::instance().getText("about_dialog_project"),
        this
    );
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);

    // // Bouton fermer
    // QPushButton *closeButton = new QPushButton(PrefManager::instance().getText("close"), this);

    // layout->addWidget(closeButton, 0, Qt::AlignRight);

    // connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);

}

AboutWidget::~AboutWidget()
{
}
