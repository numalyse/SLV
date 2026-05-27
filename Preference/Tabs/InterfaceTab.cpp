#include "InterfaceTab.h"

#include "PrefManager.h"
#include "Preference/Frames/FormComboBoxFrame.h"


#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : BasePreferenceTab("Interface", parent) 
{
    auto& prefManager = PrefManager::instance();

    QWidget* titleWidget = new QWidget(this);
    titleWidget->setFixedHeight(30);
    titleWidget->setContentsMargins(0,0,0,0);
    titleWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0,0,0,0);
    titleWidget->setLayout(titleLayout);

    QLabel* tabTitle = new QLabel;
    tabTitle->setText(PrefManager::instance().getText("tab_param_general"));

    QFont tabTitleFont = tabTitle->font();
    tabTitleFont.setPointSize(12);
    tabTitleFont.setBold(true);
    tabTitle->setFont(tabTitleFont);

    titleLayout->addWidget(tabTitle);
    titleLayout->addStretch();

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_containerLayout->insertWidget(0, line);
    m_containerLayout->insertWidget(0, titleWidget);

    QString preferredLang = prefManager.getPref("Interface", "Lang", "code");
    QStringList availableLangs = prefManager.getAvailableLangs();

    FormComboBoxFrame* langFrame = new FormComboBoxFrame(
        prefManager.getText("interface_lang_code"), 
        "Lang",
        "code",    
        preferredLang, 
        availableLangs, 
        m_container
    ); 

    addPreferenceFrame(langFrame);

    m_layout->addRow(langFrame);
 
    m_containerLayout->addStretch();
}
