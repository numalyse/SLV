#include "PathTab.h"

#include "Preference/Frames/FormPathEditFrame.h"
#include "PrefManager.h"

#include <QDir>

PathTab::PathTab(QWidget *parent) : BasePreferenceTab("Paths", parent)
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
    tabTitle->setText(PrefManager::instance().getText("tab_param_paths"));

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
    
    FormPathEditFrame* defaultVideoPath = new FormPathEditFrame(
        prefManager.getText("paths_default_video"),          
        "Paths",                      
        "default",           
        prefManager.getPref("Paths", "default"),
        false,
        true,                           
        this
    );


    FormPathEditFrame* screenshotPath = new FormPathEditFrame(
        prefManager.getText("paths_screenshot"),          
        "Paths",                      
        "screenshot",           
        prefManager.getPref("Paths", "screenshot"),
        true,
        true,                           
        this
    );

    screenshotPath->addResetButton(QDir::homePath() + "/NumalysePlayer_Content/Captures_Images");


    defaultVideoPath->setToolTip( prefManager.getText("paths_screenshot_tooltip"));

    addPreferenceFrame(defaultVideoPath);
    addPreferenceFrame(screenshotPath);

    m_containerLayout->addStretch();
}