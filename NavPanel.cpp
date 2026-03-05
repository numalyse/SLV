#include "NavPanel.h"
#include <QLabel.h>

NavPanel::NavPanel(QWidget *parent)
    : QWidget{parent}
{


    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QVBoxLayout *sideWidgetLayout = new QVBoxLayout();
    QLabel *label1 = new QLabel("Test pour l'arrangement du navpanel");
    QLabel *label2 = new QLabel("2ème test pour l'arrangement du navpanel");
    sideWidgetLayout->addWidget(label1);
    sideWidgetLayout->addWidget(label2);
    m_sideWidget = new QWidget();
    m_sideWidget->setLayout(sideWidgetLayout);
    mainLayout->addWidget(m_sideWidget);
    setStyleSheet("border: 1px solid grey;");
    m_sideWidget->hide();
}

void NavPanel::showPanel()
{
    qDebug() << "bouton cliqué";
    m_sideWidget->show();
    setFixedWidth(300);
}

void NavPanel::hidePanel()
{
    m_sideWidget->hide();
    setFixedWidth(0);
}
