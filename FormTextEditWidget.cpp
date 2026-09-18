#include "FormTextEditWidget.h"
#include <QStyleHints>
#include <QGuiApplication>

FormTextEditWidget::FormTextEditWidget(const QString &name, const QString &text, bool editable, QWidget *parent) : QFrame(parent)
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

    m_frameLayout = new QVBoxLayout(this);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignLeft);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_nameLabel->setText(name);

    m_textEdit = new FormTextEdit(text, editable, this);
    m_textEdit->setAlignment(Qt::AlignLeft);

    m_frameLayout->addWidget(m_nameLabel);
    m_frameLayout->addWidget(m_textEdit);
}

void FormTextEditWidget::setText(const QString &newText)
{
    m_textEdit->setText(newText);
}
