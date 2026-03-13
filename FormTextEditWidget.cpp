#include "FormTextEditWidget.h"

FormTextEditWidget::FormTextEditWidget(const QString &name, const QString &text, bool editable, QWidget *parent)
{
    setAutoFillBackground(true);
    setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");
        
    m_frameLayout = new QVBoxLayout(this);
    m_frameLayout->setContentsMargins(0, 0, 0, 0);
    m_frameLayout->setSpacing(2);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignLeft);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_nameLabel->setText(name);

    m_textEdit = new FormTextEdit(text, editable, this);
    m_textEdit->setAlignment(Qt::AlignRight);
    
    m_frameLayout->addWidget(m_nameLabel);
    m_frameLayout->addWidget(m_textEdit);
}

void FormTextEditWidget::setText(const QString &newText)
{
    m_textEdit->setText(newText);
}
