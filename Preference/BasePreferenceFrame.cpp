#include "BasePreferenceFrame.h"

BasePreferenceFrame::BasePreferenceFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
: QFrame(parent), m_subCategory(subCategory), m_key(key), m_prevValue(value)
{
    setAutoFillBackground(true);
    setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");

    m_layout = new QFormLayout(this);

    QLabel* nameLabel = new QLabel(name, this);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setStyleSheet("font-weight: bold;");

    m_layout->setWidget(0, QFormLayout::LabelRole, nameLabel);
}

void BasePreferenceFrame::setRightWidget(QWidget* rightWidget) {
    m_layout->setWidget(0, QFormLayout::FieldRole, rightWidget);
}

void BasePreferenceFrame::setRightLayout(QLayout* rightLayout) {
    m_layout->setLayout(0, QFormLayout::FieldRole, rightLayout);
}
