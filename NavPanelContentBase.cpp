#include "NavPanelContentBase.h"

NavPanelContentBase::NavPanelContentBase(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    int left, top, right, bottom;
    mainLayout->getContentsMargins(&left, &top, &right, &bottom);
    mainLayout->setContentsMargins(left, 0, right, 0);
    //mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *headerContainer = new QWidget(this);
    m_headerLayout = new QVBoxLayout(headerContainer);
    mainLayout->addWidget(headerContainer);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *scrollContent = new QWidget;
    m_scrollLayout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1); 

    auto *footerContainer = new QWidget(this);
    //footerContainer->setContentsMargins(0, 0, 0, 0);
    //footerContainer->setStyleSheet("background-color: green ;");
    m_footerLayout = new QVBoxLayout(footerContainer);
    mainLayout->addWidget(footerContainer);
}