#include "AboutWidget.h"
#include "PrefManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutWidget::AboutWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(PrefManager::instance().getText("about_dialog_title"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *appIcon = new QLabel();
    appIcon->setPixmap(QPixmap(":/logo/numalyse_logo_white").scaled(40, 40));
    appIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(appIcon);

    // Titre / nom de l'app
    QLabel *title = new QLabel("<h2>Numalyse Player</h2>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Description
    QLabel *desc = new QLabel(
        PrefManager::instance().getText("about_dialog_project") + "\n" +
        PrefManager::instance().getText("about_dialog_qt") + "\n" +
        PrefManager::instance().getText("about_dialog_version"),
        // "Développé avec Qt.<br><br>"
        // "© 2026 - Tous droits réservés.",
        this
    );
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);

    // Bouton fermer
    QPushButton *closeButton = new QPushButton(PrefManager::instance().getText("close"), this);

    layout->addWidget(closeButton, 0, Qt::AlignRight);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);

    resize(300, 200);
}

AboutWidget::~AboutWidget()
{
}
