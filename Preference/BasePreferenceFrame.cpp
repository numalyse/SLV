#include "BasePreferenceFrame.h"
#include "PrefManager.h"

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

    m_layoutHB = new QHBoxLayout(this);

    QLabel* nameLabel = new QLabel(name, this);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setStyleSheet("font-weight: bold;");

    m_layoutHB->addWidget(nameLabel);
    m_layoutHB->setAlignment(nameLabel, Qt::AlignLeft);

    m_rightContainer = new QWidget(this);
    m_rightLayout = new QHBoxLayout(m_rightContainer);
    m_rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightLayout->setSpacing(5);

    m_layoutHB->addWidget(m_rightContainer);
    m_layoutHB->setAlignment(m_rightContainer, Qt::AlignRight);
}

void BasePreferenceFrame::addResetButton(const QString& defaultValue){
    m_defaultValue = defaultValue;
    if (m_resetBtn) return;

    m_resetBtn = new ToolbarButton(this, "reset_setting_white", PrefManager::instance().getText("restore_defaults"));
    m_resetBtn->setCursor(Qt::PointingHandCursor);

    
    const bool dark = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    const QString borderColor = dark ? "palette(midlight)" : "palette(mid)";
    const QString bgColor     = dark ? "palette(button)"   : "palette(window)";

    m_resetBtn->setStyleSheet(
        "ToolbarButton {"
        "   border: 1px solid " + borderColor + ";"
        "   border-radius: 4px;"
        "   background-color: " + bgColor + ";"
        "}"
        "ToolbarButton:hover {"
        "   border: 1px solid " + borderColor + ";"
        "   background-color: palette(midlight);"
        "}");

    m_rightLayout->addWidget(m_resetBtn);

    connect(m_resetBtn, &QPushButton::clicked, this, [this]() {
        if (m_prevValue == m_defaultValue) return;
        setUIValue(m_defaultValue);
        emit updateJsonObjRequested(m_subCategory, m_key, m_defaultValue);
    });
}

void BasePreferenceFrame::addRightWidget(QWidget* rightWidget) {
    m_rightLayout->addWidget(rightWidget);
}


void BasePreferenceFrame::applyFrameStyleToChild(QWidget* child) {
    
    child->setStyleSheet(R"(
            border: none; 
            background-color: )" + backgroundFillColor + R"(; 
            padding: 1px; 
            border-radius: 5px;
    )");
}