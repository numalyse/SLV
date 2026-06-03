#include "BasePreferenceFrame.h"
#include <QStyleHints>
#include <QGuiApplication>

BasePreferenceFrame::BasePreferenceFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent)
: QFrame(parent), m_subCategory(subCategory), m_key(key), m_prevValue(value)
{

#ifdef Q_OS_MAC
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        backgroundFillColor = "palette(mid)";
    } else {
        backgroundFillColor = "palette(base)";
    }
#else
    backgroundFillColor = "palette(base)";
#endif

    setAutoFillBackground(true);
    setStyleSheet(R"(
        BasePreferenceFrame {
            border: none; 
            background-color: )" + backgroundFillColor + R"(; 
            padding: 1px; 
            border-radius: 5px;
        }
    )");

    //m_layout = new QFormLayout(this);
    m_layoutHB = new QHBoxLayout(this);

    QLabel* nameLabel = new QLabel(name, this);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setStyleSheet("font-weight: bold;");

    //m_layout->setWidget(0, QFormLayout::LabelRole, nameLabel);
    m_layoutHB->addWidget(nameLabel);
    m_layoutHB->setAlignment(nameLabel, Qt::AlignLeft);

}

void BasePreferenceFrame::setRightWidget(QWidget* rightWidget) {
    //m_layout->setWidget(0, QFormLayout::FieldRole, rightWidget);
    m_layoutHB->addWidget(rightWidget);
    m_layoutHB->setAlignment(rightWidget, Qt::AlignRight);
}

void BasePreferenceFrame::setRightLayout(QLayout* rightLayout) {
    //m_layout->setLayout(0, QFormLayout::FieldRole, rightLayout);

}

void BasePreferenceFrame::applyFrameStyleToChild(QWidget* child) {
    
    child->setStyleSheet(R"(
            border: none; 
            background-color: )" + backgroundFillColor + R"(; 
            padding: 1px; 
            border-radius: 5px;
    )");
}