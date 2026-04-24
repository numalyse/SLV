#include "HelperWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

HelperWidget::HelperWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Aide");

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Exemple de contenu
    QLabel *label = new QLabel("Bienvenue dans l'aide de l'application.", this);
    layout->addWidget(label);

    // Bouton fermer
    QPushButton *closeButton = new QPushButton("Fermer", this);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);
}

HelperWidget::~HelperWidget()
{
}