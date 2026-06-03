#include "FormLineEditWidget.h"
#include <QStyleHints>
#include <QGuiApplication>


FormLineEditWidget::FormLineEditWidget(const QString &name, const QString &text, bool editable, QWidget *parent) : QFrame(parent)
{
    setAutoFillBackground(true);
    QString backgroundFillColor;

#ifdef Q_OS_MAC
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        backgroundFillColor = "palette(mid)";
    } else {
        backgroundFillColor = "palette(base)";
    }
#else
    backgroundFillColor = "palette(base)";
#endif

    setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 1px; border-radius: 5px;");

    //m_frameLayout = new QFormLayout(this);
    m_frameLayout = new QHBoxLayout(this);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignLeft);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_nameLabel->setText(name);

    m_lineEdit = new FormLineEdit(text, editable, this);
    m_lineEdit->setAlignment(Qt::AlignRight);
    
    //m_frameLayout->addRow(m_nameLabel, m_lineEdit);
    m_frameLayout->addWidget(m_nameLabel);
    m_frameLayout->addWidget(m_lineEdit);
}

void FormLineEditWidget::setText(const QString &newText)
{
    m_lineEdit->setText(newText);
}
