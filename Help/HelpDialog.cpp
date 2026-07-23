#include "HelpDialog.h"

#include "Base/CategoryBase.h"
#include "Content/GeneralContent.h"
#include "Content/MonoviewContent.h"
#include "Content/MultiviewContent.h"
#include "Content/ToolbarsContent.h"

#include "PrefManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QString>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QGuiApplication>
#include <QScreen>

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
    PrefManager pref = PrefManager::instance();
    setWindowTitle(pref.getText("help_dialog_title"));

    int fixedDialogWidth = double(QGuiApplication::primaryScreen()->size().width()) * 0.5;
    int fixedDialogHeight = double(QGuiApplication::primaryScreen()->size().height()) * 0.75;
    setFixedSize(fixedDialogWidth, fixedDialogHeight);

    m_helpDialogLayout = new QHBoxLayout(this);

    // Side Menu
    m_sideMenuWidget = new QWidget();
    m_sideMenuLayout = new QVBoxLayout(m_sideMenuWidget);
    m_sideMenuLayout->setContentsMargins(0, 0, 0, 0);
    m_sideMenuLayout->setSpacing(0);

    m_sideMenuTreeWidget = new QTreeWidget();
    m_sideMenuTreeWidget->setHeaderLabel(pref.getText("help"));
    m_sideMenuTreeWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    m_sideMenuLayout->addWidget(m_sideMenuTreeWidget);

    m_helpDialogLayout->addWidget(m_sideMenuWidget, 1);

    // Contenu
    m_contentWidget = new QStackedWidget();
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //m_contentWidget->setStyleSheet("background-color : blue;");

    m_helpDialogLayout->addWidget(m_contentWidget, 4);

    auto* general = new GeneralContent(this);
    general->populateTree(m_sideMenuTreeWidget);

    auto* monoview = new MonoviewContent(this);
    monoview->populateTree(m_sideMenuTreeWidget);

    auto* multiview = new MultiviewContent(this);
    multiview->populateTree(m_sideMenuTreeWidget);

    auto* tooltbars = new ToolbarsContent(this);
    tooltbars->populateTree(m_sideMenuTreeWidget);

    m_sideMenuTreeWidget->topLevelItem(0)->setExpanded(true);


    connect(m_sideMenuTreeWidget, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int)
    {
        QWidget* widget = item->data(0, Qt::UserRole).value<QWidget*>();

        if(widget)
            showContent(widget);
    });
}

HelpDialog::~HelpDialog()
{
}

void HelpDialog::showContent(QWidget* widget)
{
    int index = m_contentWidget->indexOf(widget);

    if(index == -1)
        index = m_contentWidget->addWidget(widget);

    m_contentWidget->setCurrentIndex(index);
}