#include "InterfaceTab.h"

#include "PrefManager.h"
#include "Preference/Frames/FormComboBoxFrame.h"


#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>

InterfaceTab::InterfaceTab(QWidget *parent) : BasePreferenceTab("General", parent) 
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
    tabTitle->setText(PrefManager::instance().getText("tab_param_interface"));

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

    QString preferredLang = prefManager.getPref("General", "Lang", "code");
    QStringList availableLangs = prefManager.getAvailableLangs();

    FormComboBoxFrame* langFrame = new FormComboBoxFrame(
        prefManager.getText("general_lang_code"), 
        "Lang",
        "code",    
        preferredLang, 
        availableLangs, 
        m_container
    ); 

    addPreferenceFrame(langFrame);

    QLabel* timelineOptionsLabel = new QLabel(prefManager.getText("Advanced_timeline_options"));
    m_containerLayout->addWidget(timelineOptionsLabel);

    QString shotImageType = prefManager.getPref("General", "Advanced_timeline_options", "general_timeline_shot_image");
    FormComboBoxFrame* timelineTagFrame = new FormComboBoxFrame(
        prefManager.getText("general_timeline_shot_image"),
        "Advanced_timeline_options",
        "general_timeline_shot_image",
        shotImageType,
        QStringList({"shot_start_image", "shot_tag_image"}),
        m_container
    );

    timelineTagFrame->comboBox()->setItemData(0, prefManager.getText("shot_start_image_tooltip"), Qt::ToolTipRole);

    addPreferenceFrame(timelineTagFrame);

    QLabel* subTabTitle = new QLabel;
    subTabTitle->setText(PrefManager::instance().getText("tab_param_export"));

    subTabTitle->setFont(tabTitleFont);

    m_containerLayout->addWidget(line);
    m_containerLayout->addWidget(subTabTitle);

    QString userAudioPreference = PrefManager::instance().getPref("General", "Exports", "sequence_extraction_audio_format");

    //Exports sequence_extraction_audio_format
    FormComboBoxFrame* audioFormatType = new FormComboBoxFrame(
        prefManager.getText("sequence_extraction_audio_format"),
        "Exports",
        "sequence_extraction_audio_format",
        userAudioPreference,
        QStringList({"audio_format_mp3", "audio_format_aac"}),
        m_container
    );

    addPreferenceFrame(audioFormatType);
    
    m_containerLayout->addStretch();
}
