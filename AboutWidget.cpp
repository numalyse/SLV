#include "AboutWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutWidget::AboutWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("À propos");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Titre / nom de l'app
    QLabel *title = new QLabel("<h2>App</h2>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Description
    QLabel *desc = new QLabel(
        "Version 1.0.0<br>"
        "Développé avec Qt.<br><br>"
        "© 2026 - Tous droits réservés.",
        this
    );
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);

    // Bouton fermer
    QPushButton *closeButton = new QPushButton("Fermer", this);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);

    resize(300, 200);
}

AboutWidget::~AboutWidget()
{
}