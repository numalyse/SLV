#include "FormLineEditWidget.h"


FormLineEditWidget::FormLineEditWidget(const QString &name, const QString &text, bool editable, QWidget *parent)
{
    setAutoFillBackground(true);
    setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");

    m_frameLayout = new QFormLayout(this);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignLeft);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_nameLabel->setText(name);

    m_lineEdit = new FormLineEdit(text, editable, this);
    m_lineEdit->setAlignment(Qt::AlignRight);
    
    m_frameLayout->addRow(m_nameLabel, m_lineEdit);
}

void FormLineEditWidget::setText(const QString &newText)
{
    m_lineEdit->setText(newText);
}
