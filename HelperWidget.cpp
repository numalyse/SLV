#include "HelperWidget.h"

#include "PrefManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

HelperWidget::HelperWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(PrefManager::instance().getText("help_dialog_title"));

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Exemple de contenu
    QLabel *label = new QLabel(
        PrefManager::instance().getText("help_dialog_wip") + "\n" +
        PrefManager::instance().getText("help_dialog_report") + "\n" +
        "christina.maurin@umpv.fr \n"
        "killian.viguier@umpv.fr",
        this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    // Bouton fermer
    QPushButton *closeButton = new QPushButton(PrefManager::instance().getText("close"), this);
    layout->addWidget(closeButton, 0, Qt::AlignRight);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);
}

HelperWidget::~HelperWidget()
{
}
