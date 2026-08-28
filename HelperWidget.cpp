#include "HelperWidget.h"

#include "PrefManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QString>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QGuiApplication>
#include <QScreen>

HelperWidget::HelperWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(PrefManager::instance().getText("help_dialog_title"));
    int fixedDialogWidth = double(QGuiApplication::primaryScreen()->size().width()) * 0.5;
    int fixedDialogHeight = double(QGuiApplication::primaryScreen()->size().height()) * 0.5;
    setFixedSize(fixedDialogWidth, fixedDialogHeight);
    initLayout();
    setGeneralContent();

}

HelperWidget::~HelperWidget()
{
}

void HelperWidget::initLayout()
{
    PrefManager pref = PrefManager::instance();
    m_mainWidget = new QWidget(this);
    m_mainWidget->setContentsMargins(0,0,0,0);

    m_menuScrollArea = new QScrollArea();
    // m_menuScrollArea->setMaximumWidth(220);
    m_menuScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *mainLayout = new QHBoxLayout();

    m_sideMenu = new QWidget();
    m_sideMenu->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    QVBoxLayout *sideMenuLayout = new QVBoxLayout();

    m_generalTab = new CustomCheckbox("<b>" + pref.getText("help_menu_general_checkbox") + "</b>");

    QWidget *generalSubTab = new QWidget();
    QVBoxLayout *generalSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *reportIssuesLabel = new CustomHoverLabel(pref.getText("help_menu_report_issues_label"));
    generalSubTabLayout->addWidget(reportIssuesLabel);
    generalSubTab->setLayout(generalSubTabLayout);

    connect(m_generalTab, &CustomCheckbox::stateChanged, this, [this, generalSubTab](const bool state){
        animateWidget(generalSubTab, state);
    });

    m_classicTab = new CustomCheckbox("<b>" + pref.getText("help_menu_classic_category") + "</b>");

    QWidget *classicSubTab = new QWidget();
    QVBoxLayout *classicSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *openFilesLabel = new CustomHoverLabel(pref.getText("help_menu_open_files_label"));
    CustomHoverLabel *classicToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_classic_toolbar_label"));
    CustomHoverLabel *extendedToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_extended_toolbar_label"));
    classicSubTabLayout->addWidget(openFilesLabel);
    classicSubTabLayout->addWidget(classicToolbarLabel);
    classicSubTabLayout->addWidget(extendedToolbarLabel);
    classicSubTab->setLayout(classicSubTabLayout);
    m_classicTab->setUnchecked();
    connect(m_classicTab, &CustomCheckbox::stateChanged, this, [this, classicSubTab](const bool state){
        animateWidget(classicSubTab, state);
    });
    emit m_classicTab->stateChanged(false);

    m_multiviewTab = new CustomCheckbox("<b>" + pref.getText("help_menu_multiview_category") + "</b>");

    QWidget *multiviewSubTab = new QWidget();
    QVBoxLayout *multiviewSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *enterMultiviewLabel = new CustomHoverLabel(pref.getText("help_menu_enter_multiview_label"));
    CustomHoverLabel *simplifiedToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_simplified_toolbar_label"));
    CustomHoverLabel *globalToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_global_toolbar_label"));
    multiviewSubTabLayout->addWidget(enterMultiviewLabel);
    multiviewSubTabLayout->addWidget(simplifiedToolbarLabel);
    multiviewSubTabLayout->addWidget(globalToolbarLabel);
    multiviewSubTab->setLayout(multiviewSubTabLayout);
    m_multiviewTab->setUnchecked();
    connect(m_multiviewTab, &CustomCheckbox::stateChanged, this, [this, multiviewSubTab](const bool state){
        animateWidget(multiviewSubTab, state);
    });
    emit m_multiviewTab->stateChanged(false);

    m_playlistTab = new CustomCheckbox("<b>" +pref.getText("help_menu_playlist_category") + "</b>");

    QWidget *playlistSubTab = new QWidget();
    QVBoxLayout *playlistSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *accessPlaylistLabel = new CustomHoverLabel(pref.getText("help_menu_access_playlist_label"));
    CustomHoverLabel *addElementsPlaylistLabel = new CustomHoverLabel(pref.getText("help_menu_add_elements_playlist_label"));
    CustomHoverLabel *playlistButtonsLabel = new CustomHoverLabel(pref.getText("help_menu_playlist_buttons_label"));
    playlistSubTabLayout->addWidget(accessPlaylistLabel);
    playlistSubTabLayout->addWidget(addElementsPlaylistLabel);
    playlistSubTabLayout->addWidget(playlistButtonsLabel);
    playlistSubTab->setLayout(playlistSubTabLayout);
    m_playlistTab->setUnchecked();
    connect(m_playlistTab, &CustomCheckbox::stateChanged, this, [this, playlistSubTab](const bool state){
        animateWidget(playlistSubTab, state);
    });
    emit m_playlistTab->stateChanged(false);

    m_timelineTab = new CustomCheckbox("<b>" +pref.getText("help_menu_timeline_category") + "</b>");

    QWidget *timelineSubTab = new QWidget();
    QVBoxLayout *timelineSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *timelineDisplayLabel = new CustomHoverLabel(pref.getText("help_menu_timeline_display_label"));
    CustomHoverLabel *timelineButtonsLabel = new CustomHoverLabel(pref.getText("help_menu_timeline_buttons_label"));
    CustomHoverLabel *shotInfosLabel = new CustomHoverLabel(pref.getText("help_menu_shot_infos_label"));
    CustomHoverLabel *exportationLabel = new CustomHoverLabel(pref.getText("help_menu_exportation_label"));
    timelineSubTabLayout->addWidget(timelineDisplayLabel);
    timelineSubTabLayout->addWidget(timelineButtonsLabel);
    timelineSubTabLayout->addWidget(shotInfosLabel);
    timelineSubTabLayout->addWidget(exportationLabel);
    timelineSubTab->setLayout(timelineSubTabLayout);
    m_timelineTab->setUnchecked();
    connect(m_timelineTab, &CustomCheckbox::stateChanged, this, [this, timelineSubTab](const bool state){
        animateWidget(timelineSubTab, state);
    });
    emit m_timelineTab->stateChanged(false);

    sideMenuLayout->addWidget(m_generalTab);

    sideMenuLayout->addWidget(generalSubTab);
    sideMenuLayout->addWidget(m_classicTab);
    sideMenuLayout->addWidget(classicSubTab);

    sideMenuLayout->addWidget(m_multiviewTab);
    sideMenuLayout->addWidget(multiviewSubTab);
    sideMenuLayout->addWidget(m_playlistTab);

    sideMenuLayout->addWidget(playlistSubTab);
    sideMenuLayout->addWidget(m_timelineTab);
    sideMenuLayout->addWidget(timelineSubTab);

    sideMenuLayout->addStretch();

    m_sideMenu->setLayout(sideMenuLayout);
    m_menuScrollArea->setWidget(m_sideMenu);
    m_menuScrollArea->setWidgetResizable(true);
    mainLayout->addWidget(m_menuScrollArea);

    m_scrollArea = new QScrollArea();

    mainLayout->addWidget(m_scrollArea, 3);

    m_mainWidget->setLayout(mainLayout);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_mainWidget);

    connect(reportIssuesLabel, &CustomHoverLabel::clicked, this, &HelperWidget::setGeneralContent);

    connect(openFilesLabel, &CustomHoverLabel::clicked, this, [this](){setClassicContent(0);});
    connect(classicToolbarLabel, &CustomHoverLabel::clicked, this, [this](){setClassicContent(1);});
    connect(extendedToolbarLabel, &CustomHoverLabel::clicked, this, [this](){setClassicContent(2);});

    connect(enterMultiviewLabel, &CustomHoverLabel::clicked, this, [this](){setMultiviewContent(0);});
    connect(simplifiedToolbarLabel, &CustomHoverLabel::clicked, this, [this](){setMultiviewContent(1);});
    connect(globalToolbarLabel, &CustomHoverLabel::clicked, this, [this](){setMultiviewContent(2);});

    connect(accessPlaylistLabel, &CustomHoverLabel::clicked, this, [this](){setPlaylistContent(0);});
    connect(addElementsPlaylistLabel, &CustomHoverLabel::clicked, this, [this](){setPlaylistContent(1);});
    connect(playlistButtonsLabel, &CustomHoverLabel::clicked, this, [this](){setPlaylistContent(2);});

    connect(timelineDisplayLabel, &CustomHoverLabel::clicked, this, [this](){setTimelineContent(0);});
    connect(timelineButtonsLabel, &CustomHoverLabel::clicked, this, [this](){setTimelineContent(1);});
    connect(shotInfosLabel, &CustomHoverLabel::clicked, this, [this](){setTimelineContent(2);});
    connect(exportationLabel, &CustomHoverLabel::clicked, this, [this](){setTimelineContent(3);});

    m_imageWidth = (m_scrollArea->viewport()->width() - 30) * 0.85;

}

// ------
// ------ PANNEAU : GENERAL
// ------
void HelperWidget::setGeneralContent()
{
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(20, 40, 20, 40);

    QLabel *text = new QLabel(PrefManager::instance().getText("help_dialog_wip"), this);
    text->setAlignment(Qt::AlignCenter);
    text->setStyleSheet("font-style: italic;");
    layout->addWidget(text);

    layout->addSpacing(40);

    QLabel *report = new QLabel(PrefManager::instance().getText("help_dialog_report"), this);
    report->setAlignment(Qt::AlignCenter);
    layout->addWidget(report);

    QString mails =
        QStringLiteral("<a href=\"christina.maurin@umpv.fr\">christina.maurin@umpv.fr</a>") +
        QStringLiteral("<br>") +
        QStringLiteral("<a href=\"mateusz.birembaut@umpv.fr\">mateusz.birembaut@umpv.fr</a>") +
        QStringLiteral("<br>") +
        QStringLiteral("<a href=\"killian.viguier@umpv.fr\">killian.viguier@umpv.fr</a>");

    QLabel *label_mails = new QLabel(mails, this);
    label_mails->setAlignment(Qt::AlignCenter);
    label_mails->setTextFormat(Qt::RichText);
    label_mails->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label_mails->setOpenExternalLinks(true);
    label_mails->setStyleSheet("a { text-decoration: none; }");
    layout->addWidget(label_mails);



    // // Bouton fermer
    // QPushButton *closeButton = new QPushButton(PrefManager::instance().getText("close"), this);
    // layout->addWidget(closeButton, 0, Qt::AlignRight);

    // connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    m_contentWidget->setLayout(layout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
}

// ------
// ------ PANNEAU : MODE CLASSIQUE
// ------
void HelperWidget::setClassicContent(int scrollLevel)
{
    PrefManager pref = PrefManager::instance();
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QLabel* classicContentTitle = new QLabel("<h1><b>" + pref.getText("help_menu_classic_category") + "</b></h1>");

    QLabel* openFilesTitle = new QLabel("<h2><b>" + pref.getText("help_menu_open_files_label") + "</b></h2>");

    QLabel* openFilesContent1 = new QLabel(pref.getText("help_menu_open_files_content_1"));
    openFilesContent1->setWordWrap(true);

    QLabel* openFilesContentIllustration1 = new QLabel();
    QPixmap illustration1(":/help_dialog_illustrations/open_medias");
    openFilesContentIllustration1->setPixmap(illustration1.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    // openFilesContentIllustration1->setScaledContents(true);

    QLabel* openFilesContent2 = new QLabel(pref.getText("help_menu_open_files_content_2"));
    openFilesContent2->setWordWrap(true);

    QLabel* openFilesContentIllustration2 = new QLabel();
    QPixmap illustration2(":/help_dialog_illustrations/open_medias_drag_drop");
    openFilesContentIllustration2->setPixmap(illustration2.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    layout->addSpacing(10);
    layout->addWidget(classicContentTitle, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(openFilesTitle);
    layout->addSpacing(20);
    layout->addWidget(openFilesContent1);
    layout->addSpacing(20);
    layout->addWidget(openFilesContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(openFilesContent2);
    layout->addSpacing(20);
    layout->addWidget(openFilesContentIllustration2, 0, Qt::AlignCenter);

    QLabel* classicToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_classic_toolbar_label") + "</b></h2>");

    QLabel* classicToolbarContent1 = new QLabel(pref.getText("help_menu_classic_toolbar_content_1"));

    QLabel* classicToolbarContentIllustration1 = new QLabel();
    QPixmap classicToolbarIllustration1(":/help_dialog_illustrations/monoview_toolbar_display");
    classicToolbarContentIllustration1->setPixmap(classicToolbarIllustration1.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    QLabel* classicToolbarButtonsLabel = new QLabel("<h3>" + pref.getText("help_menu_classic_toolbar_buttons_description") + "</h3>");

    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";
    QGridLayout* classicToolbarButtonsHelpLayout = new QGridLayout();
    //classicToolbarButtonsHelpLayout->setSpacing(30);
    classicToolbarButtonsHelpLayout->setContentsMargins(30, 10, 30, 10);
    QLabel* playInfoIcon = new QLabel();
    playInfoIcon->setPixmap(QPixmap(":/icons/play" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* pauseInfoIcon = new QLabel();
    pauseInfoIcon->setPixmap(QPixmap(":/icons/pause" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QHBoxLayout* playPauseIcons = new QHBoxLayout();
    playPauseIcons->setSpacing(5);
    playPauseIcons->addWidget(playInfoIcon);
    playPauseIcons->addWidget(pauseInfoIcon);
    QLabel* playPauseLabel = new QLabel("<b>" + pref.getText("help_menu_play_pause_label") + "</b>");
    QLabel* playPauseDescription = new QLabel(pref.getText("help_menu_play_pause_description"));
    playPauseDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    playPauseDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addLayout(playPauseIcons, 0, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(playPauseLabel, 0, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(playPauseDescription, 0, 2, Qt::AlignLeft);
    // classicToolbarButtonsHelpLayout->setColumnStretch(2, 1);

    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/stop" + theme, pref.getText("help_menu_stop_label"), pref.getText("help_menu_stop_description"), 1);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/eject" + theme, pref.getText("help_menu_eject_label"), pref.getText("help_menu_eject_description"), 2);

    QLabel* prevMediaInfoIcon = new QLabel();
    prevMediaInfoIcon->setPixmap(QPixmap(":/icons/prev" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* nextMediaInfoIcon = new QLabel();
    nextMediaInfoIcon->setPixmap(QPixmap(":/icons/next" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QHBoxLayout* changeMediaIcons = new QHBoxLayout();
    changeMediaIcons->setSpacing(10);
    changeMediaIcons->addWidget(prevMediaInfoIcon);
    changeMediaIcons->addWidget(nextMediaInfoIcon);
    QLabel* changeMediaLabel = new QLabel("<b>" + pref.getText("help_menu_change_media_label") + "</b>");
    QLabel* changeMediaDescription = new QLabel(pref.getText("help_menu_change_media_description"));
    changeMediaDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    changeMediaDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addLayout(changeMediaIcons, 3, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(changeMediaLabel, 3, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(changeMediaDescription, 3, 2, Qt::AlignLeft);

    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/slow" + theme, pref.getText("help_menu_slow_label"), pref.getText("help_menu_slow_description"), 4);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/loop_off" + theme, pref.getText("help_menu_loop_off_label"), pref.getText("help_menu_loop_off_description"), 5);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/sound_on" + theme, pref.getText("help_menu_sound_on_label"), pref.getText("help_menu_sound_on_description"), 6);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/lang" + theme, pref.getText("help_menu_lang_label"), pref.getText("help_menu_lang_description"), 7);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/media_info" + theme, pref.getText("help_menu_media_info_label"), pref.getText("help_menu_media_info_description"), 8);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/zoom" + theme, pref.getText("help_menu_zoom_label"), pref.getText("help_menu_zoom_description"), 9);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/capture" + theme, pref.getText("help_menu_capture_label"), pref.getText("help_menu_capture_description"), 10);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/extract_sequence" + theme, pref.getText("help_menu_extract_sequence_label"), pref.getText("help_menu_extract_sequence_description"), 11);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/duplicate_media" + theme, pref.getText("help_menu_duplicate_media_label"), pref.getText("help_menu_duplicate_media_description"), 12);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/fullscreen" + theme, pref.getText("help_menu_fullscreen_label"), pref.getText("help_menu_fullscreen_description"), 13);
    addButtonDescription(classicToolbarButtonsHelpLayout, ":/icons/right_arrow" + theme, pref.getText("help_menu_right_arrow_label"), pref.getText("help_menu_right_arrow_description"), 14);

    layout->addSpacing(10);
    layout->addWidget(classicToolbarTitle);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarContent1);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarButtonsLabel);
    layout->addSpacing(10);
    layout->addLayout(classicToolbarButtonsHelpLayout);


    QLabel* extendedToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_extended_toolbar_label") + "</b></h2>");

    QGridLayout* extendedToolbarButtonsHelpLayout = new QGridLayout();
    extendedToolbarButtonsHelpLayout->setSpacing(30);
    extendedToolbarButtonsHelpLayout->setContentsMargins(30, 10, 30, 10);

    QLabel* extendedToolbarButtonsLabel = new QLabel("<h3>" + pref.getText("help_menu_extended_toolbar_buttons_description") + "</h3>");

    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/show_image" + theme, pref.getText("help_menu_show_image_label"), pref.getText("help_menu_show_image_description"), 4);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/adjustments" + theme, pref.getText("help_menu_adjustments_label"), pref.getText("help_menu_adjustments_description"), 5);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/draw" + theme, pref.getText("help_menu_draw_label"), pref.getText("help_menu_draw_description"), 6);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/compo_rule" + theme, pref.getText("help_menu_compo_rule_label"), pref.getText("help_menu_compo_rule_description"), 7);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/backward" + theme, pref.getText("help_menu_backward_label"), pref.getText("help_menu_backward_description"), 8);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/prev_frame" + theme, pref.getText("help_menu_prev_frame_label"), pref.getText("help_menu_prev_frame_description"), 9);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/record_off" + theme, pref.getText("help_menu_record_off_label"), pref.getText("help_menu_record_off_description"), 10);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/rotate" + theme, pref.getText("help_menu_rotate_label"), pref.getText("help_menu_rotate_description"), 11);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/invert_h" + theme, pref.getText("help_menu_invert_h_label"), pref.getText("help_menu_invert_h_description"), 12);
    addButtonDescription(extendedToolbarButtonsHelpLayout, ":/icons/timeline_off" + theme, pref.getText("help_menu_timeline_off_label"), pref.getText("help_menu_timeline_off_description"), 13);

    layout->addSpacing(10);
    layout->addWidget(extendedToolbarTitle);
    layout->addSpacing(20);
    layout->addWidget(extendedToolbarButtonsLabel);
    layout->addSpacing(10);
    layout->addLayout(extendedToolbarButtonsHelpLayout);

    layout->addStretch();
    m_contentWidget->setLayout(layout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
    switch(scrollLevel){
    case 0:
        m_scrollArea->verticalScrollBar()->setValue(0);
        break;
    case 1:
        m_scrollArea->verticalScrollBar()->setValue(classicToolbarTitle->y());
        break;
    case 2:
        m_scrollArea->verticalScrollBar()->setValue(extendedToolbarTitle->y());
    }
}

// ------
// ------ PANNEAU : MODE MULTIVUE
// ------
void HelperWidget::setMultiviewContent(int scrollLevel)
{
    PrefManager pref = PrefManager::instance();
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QLabel* multiviewContentTitle = new QLabel("<h1><b>" + pref.getText("help_menu_multiview_category") + "</b></h1>");
    QLabel* enterMultiviewTitle = new QLabel("<h2><b>" + pref.getText("help_menu_enter_multiview_label") + "</b></h2>");
    QLabel* enterMultiviewContent1 = new QLabel(pref.getText("help_menu_enter_multiview_content_1"));
    enterMultiviewContent1->setWordWrap(true);
    QLabel* enterMultiviewContentIllustration1 = new QLabel();
    QPixmap illustration1(":/help_dialog_illustrations/multiview_display");
    enterMultiviewContentIllustration1->setPixmap(illustration1.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    QLabel* enterMultiviewContent2 = new QLabel(pref.getText("help_menu_enter_multiview_content_2"));
    enterMultiviewContent2->setWordWrap(true);
    QLabel* enterMultiviewContentIllustration2 = new QLabel();
    QPixmap illustration2(":/help_dialog_illustrations/multiview_change_arrangement");
    enterMultiviewContentIllustration2->setPixmap(illustration2);
    QLabel* enterMultiviewContentIllustration3 = new QLabel();
    QPixmap illustration3(":/help_dialog_illustrations/multiview_change_arrangement_2");
    enterMultiviewContentIllustration3->setPixmap(illustration3);
    QHBoxLayout* illustrationLayout = new QHBoxLayout();
    illustrationLayout->addWidget(enterMultiviewContentIllustration2, 0, Qt::AlignCenter);
    illustrationLayout->addWidget(enterMultiviewContentIllustration3, 0, Qt::AlignCenter);

    layout->addSpacing(10);
    layout->addWidget(multiviewContentTitle, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(enterMultiviewTitle);
    layout->addSpacing(20);
    layout->addWidget(enterMultiviewContent1);
    layout->addSpacing(10);
    layout->addWidget(enterMultiviewContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(enterMultiviewContent2);
    layout->addSpacing(10);
    layout->addLayout(illustrationLayout);



    QLabel* simplifiedToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_simplified_toolbar_label") + "</b></h2>");

    QLabel* simplifiedToolbarContent1 = new QLabel(pref.getText("help_menu_simplified_content_1"));
    simplifiedToolbarContent1->setWordWrap(true);
    QLabel* simplifiedToolbarContentIllustration1 = new QLabel();
    QPixmap illustration4(":/help_dialog_illustrations/multiview_simplified_toolbar");
    simplifiedToolbarContentIllustration1->setPixmap(illustration4.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    layout->addSpacing(20);
    layout->addWidget(simplifiedToolbarTitle);
    layout->addSpacing(10);
    layout->addWidget(simplifiedToolbarContent1);
    layout->addSpacing(10);
    layout->addWidget(simplifiedToolbarContentIllustration1, 0, Qt::AlignCenter);

    QLabel* globalToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_global_toolbar_label") + "</b></h2>");

    QLabel* globalToolbarContent1 = new QLabel(pref.getText("help_menu_global_toolbar_content_1"));
    globalToolbarContent1->setWordWrap(true);
    QLabel* globalToolbarContentIllustration1 = new QLabel();
    QPixmap illustration5(":/help_dialog_illustrations/multiview_global_toolbar");
    globalToolbarContentIllustration1->setPixmap(illustration5.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    QLabel* globalToolbarContent2 = new QLabel(pref.getText("help_menu_global_toolbar_content_2"));
    globalToolbarContent2->setWordWrap(true);

    QLabel* globalToolbarContentIllustration2 = new QLabel();
    QPixmap illustration6(":/help_dialog_illustrations/multiview_captures");
    globalToolbarContentIllustration2->setPixmap(illustration6.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    layout->addSpacing(20);
    layout->addWidget(globalToolbarTitle);
    layout->addSpacing(10);
    layout->addWidget(globalToolbarContent1);
    layout->addSpacing(10);
    layout->addWidget(globalToolbarContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(globalToolbarContent2);
    layout->addSpacing(10);
    layout->addWidget(globalToolbarContentIllustration2, 0, Qt::AlignCenter);

    layout->addStretch();
    m_contentWidget->setLayout(layout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
    switch(scrollLevel){
    case 0:
        m_scrollArea->verticalScrollBar()->setValue(0);
        break;
    case 1:
        m_scrollArea->verticalScrollBar()->setValue(simplifiedToolbarTitle->y());
        break;
    case 2:
        m_scrollArea->verticalScrollBar()->setValue(globalToolbarTitle->y());
    }
}

// ------
// ------ PANNEAU : PLAYLIST
// ------
void HelperWidget::setPlaylistContent(int scrollLevel)
{
    PrefManager pref = PrefManager::instance();
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QLabel* playlistContentTitle = new QLabel("<h1><b>" + pref.getText("help_menu_playlist_category") + "</b></h1>");

    QLabel* accessPlaylistTitle = new QLabel("<h2><b>" + pref.getText("help_menu_access_playlist_label") + "</b></h2>");
    QLabel* accessPlaylistContent1 = new QLabel(pref.getText("help_menu_access_playlist_content_1"));
    accessPlaylistContent1->setWordWrap(true);
    QLabel* accessPlaylistContentIllustration1 = new QLabel();
    QPixmap illustration1(":/help_dialog_illustrations/playlist_display");
    accessPlaylistContentIllustration1->setPixmap(illustration1.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    layout->addSpacing(10);
    layout->addWidget(playlistContentTitle);
    layout->addSpacing(30);
    layout->addWidget(accessPlaylistTitle);
    layout->addSpacing(10);
    layout->addWidget(accessPlaylistContent1);
    layout->addSpacing(10);
    layout->addWidget(accessPlaylistContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(10);

    QLabel* manageItemsTitle = new QLabel("<h2><b>" + pref.getText("help_menu_add_elements_playlist_label") + "</b></h2>");
    QLabel* manageItemsContent1 = new QLabel(pref.getText("help_menu_manage_items_content_1"));
    manageItemsContent1->setWordWrap(true);
    // QLabel* manageItemsContentIllustration1 = new QLabel();
    // QPixmap illustration2(":/help_dialog_illustrations/playlist_display");
    // manageItemsContentIllustration1->setPixmap(illustration2.scaled(
    //     m_imageWidth,
    //     10000,
    //     Qt::KeepAspectRatio,
    //     Qt::SmoothTransformation
    //     ));

    layout->addWidget(manageItemsTitle);
    layout->addSpacing(10);
    layout->addWidget(manageItemsContent1);
    layout->addSpacing(10);

    QGridLayout* playlistButtonsHelpLayout = new QGridLayout();
    playlistButtonsHelpLayout->setSpacing(30);
    playlistButtonsHelpLayout->setContentsMargins(30, 10, 30, 10);
    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";

    QLabel* playlistButtonsTitle = new QLabel("<h2><b>" + pref.getText("help_menu_playlist_buttons_label") + "</b></h2>");

    addButtonDescription(playlistButtonsHelpLayout, ":/icons/playlist_loop" + theme, pref.getText("help_menu_playlist_loop_label"), pref.getText("help_menu_playlist_loop_description"), 0);
    addButtonDescription(playlistButtonsHelpLayout, ":/icons/shuffle" + theme, pref.getText("help_menu_playlist_shuffle_label"), pref.getText("help_menu_playlist_shuffle_description"), 1);
    addButtonDescription(playlistButtonsHelpLayout, ":/icons/sort" + theme, pref.getText("help_menu_playlist_sort_label"), pref.getText("help_menu_playlist_sort_description"), 2);
    addButtonDescription(playlistButtonsHelpLayout, ":/icons/delete" + theme, pref.getText("help_menu_playlist_delete_label"), pref.getText("help_menu_playlist_delete_description"), 3);

    layout->addWidget(playlistButtonsTitle);
    layout->addSpacing(10);
    layout->addLayout(playlistButtonsHelpLayout);

    layout->addStretch();
    m_contentWidget->setLayout(layout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
    switch(scrollLevel){
    case 0:
        m_scrollArea->verticalScrollBar()->setValue(0);
        break;
    case 1:
        m_scrollArea->verticalScrollBar()->setValue(accessPlaylistTitle->y());
        break;
    case 2:
        m_scrollArea->verticalScrollBar()->setValue(accessPlaylistTitle->y());
    }
}

// ------
// ------ PANNEAU : TIMELINE
// ------
void HelperWidget::setTimelineContent(int scrollLevel)
{
    PrefManager pref = PrefManager::instance();
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QLabel* timelineContentTitle = new QLabel("<h1><b>" + pref.getText("help_menu_timeline_category") + "</b></h1>");

    QLabel* timelineDisplayTitle = new QLabel("<h2><b>" + pref.getText("help_menu_timeline_display_label") + "</b></h2>");
    QLabel* timelineDisplayContent1 = new QLabel(pref.getText("help_menu_timeline_display_content_1"));
    timelineDisplayContent1->setWordWrap(true);
    QLabel* timelineDisplayContentIllustration1 = new QLabel();
    QPixmap illustration1(":/help_dialog_illustrations/timeline_display");
    timelineDisplayContentIllustration1->setPixmap(illustration1.scaled(
        m_imageWidth,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    QLabel* timelineDisplayContent2 = new QLabel(pref.getText("help_menu_timeline_display_content_2"));
    timelineDisplayContent2->setWordWrap(true);

    layout->addSpacing(10);
    layout->addWidget(timelineContentTitle, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(timelineDisplayTitle);
    layout->addSpacing(10);
    layout->addWidget(timelineDisplayContent1);
    layout->addSpacing(10);
    layout->addWidget(timelineDisplayContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(timelineDisplayContent2);

    QLabel* timelineButtonsTitle = new QLabel("<h2><b>" + pref.getText("help_menu_timeline_buttons_label") + "</b></h2>");
    layout->addWidget(timelineButtonsTitle);
    layout->addSpacing(10);

    QGridLayout* timelineButtonsHelpLayout = new QGridLayout();
    timelineButtonsHelpLayout->setSpacing(30);
    timelineButtonsHelpLayout->setContentsMargins(30, 10, 30, 10);
    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";

    addButtonDescription(timelineButtonsHelpLayout, ":/icons/abloop" + theme, pref.getText("help_menu_abloop_label"), pref.getText("help_menu_abloop_description"), 0);
    timelineButtonsHelpLayout->setColumnStretch(2, 0);
    addButtonDescription(timelineButtonsHelpLayout, ":/icons/auto_segmentation" + theme, pref.getText("help_menu_auto_segmentation_label"), pref.getText("help_menu_auto_segmentation_description"), 1);
    addButtonDescription(timelineButtonsHelpLayout, ":/icons/split_shot" + theme, pref.getText("help_menu_split_shot_label"), pref.getText("help_menu_split_shot_description"), 2);

    QLabel* mergeLeftIcon = new QLabel();
    mergeLeftIcon->setPixmap(QPixmap(":/icons/merge_left" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* mergeRightIcon = new QLabel();
    mergeRightIcon->setPixmap(QPixmap(":/icons/merge_right" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QHBoxLayout* mergeIcons = new QHBoxLayout();
    mergeIcons->setSpacing(10);
    mergeIcons->addWidget(mergeLeftIcon);
    mergeIcons->addWidget(mergeRightIcon);
    QLabel* mergeLabel = new QLabel("<b>" + pref.getText("help_menu_merge_label") + "</b>");
    QLabel* mergeDescription = new QLabel(pref.getText("help_menu_merge_description"));
    mergeDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mergeDescription->setWordWrap(true);
    timelineButtonsHelpLayout->addLayout(mergeIcons, 3, 0, Qt::AlignCenter);
    timelineButtonsHelpLayout->addWidget(mergeLabel, 3, 1, Qt::AlignCenter);
    timelineButtonsHelpLayout->addWidget(mergeDescription, 3, 2, Qt::AlignLeft);

    QLabel* prevShotIcon = new QLabel();
    prevShotIcon->setPixmap(QPixmap(":/icons/to_prev_shot" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* nextShotIcon = new QLabel();
    nextShotIcon->setPixmap(QPixmap(":/icons/to_next_shot" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QHBoxLayout* changeShotIcons = new QHBoxLayout();
    changeShotIcons->setSpacing(10);
    changeShotIcons->addWidget(prevShotIcon);
    changeShotIcons->addWidget(nextShotIcon);
    QLabel* changeShotLabel = new QLabel("<b>" + pref.getText("help_menu_change_shot_label") + "</b>");
    QLabel* changeShotDescription = new QLabel(pref.getText("help_menu_change_shot_description"));
    mergeDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mergeDescription->setWordWrap(true);
    timelineButtonsHelpLayout->addLayout(changeShotIcons, 4, 0, Qt::AlignCenter);
    timelineButtonsHelpLayout->addWidget(changeShotLabel, 4, 1, Qt::AlignCenter);
    timelineButtonsHelpLayout->addWidget(changeShotDescription, 4, 2, Qt::AlignLeft);

    addButtonDescription(timelineButtonsHelpLayout, ":/icons/shot_detail" + theme, pref.getText("help_menu_shot_detail_label"), pref.getText("help_menu_shot_detail_description"), 5);
    addButtonDescription(timelineButtonsHelpLayout, ":/icons/export" + theme, pref.getText("help_menu_export_label"), pref.getText("help_menu_export_description"), 6);

    layout->addLayout(timelineButtonsHelpLayout);
    layout->addSpacing(10);

    QLabel* shotInfosTitle = new QLabel("<h2><b>" + pref.getText("help_menu_shot_infos_label") + "</b></h2>");
    QLabel* shotInfosContent1 = new QLabel(pref.getText("help_menu_shot_infos_content_1"));
    shotInfosContent1->setWordWrap(true);
    QLabel* shotInfosContentIllustration1 = new QLabel();
    QPixmap illustration2(":/help_dialog_illustrations/timeline_shot_infos");
    shotInfosContentIllustration1->setPixmap(illustration2);

    layout->addWidget(shotInfosTitle);
    layout->addSpacing(10);
    layout->addWidget(shotInfosContent1);
    layout->addSpacing(10);
    layout->addWidget(shotInfosContentIllustration1, 0, Qt::AlignCenter);
    layout->addSpacing(10);

    QLabel* exportationTitle = new QLabel("<h2><b>" + pref.getText("help_menu_exportation_label") + "</b></h2>");
    QLabel* exportationContent1 = new QLabel(pref.getText("help_menu_exportation_content_1"));
    exportationContent1->setWordWrap(true);
    QLabel* exportationContentIllustration1 = new QLabel();
    QPixmap illustration3(":/help_dialog_illustrations/timeline_exportation");
    exportationContentIllustration1->setPixmap(illustration3);

    layout->addWidget(exportationTitle);
    layout->addSpacing(10);
    layout->addWidget(exportationContent1);
    layout->addSpacing(10);
    layout->addWidget(exportationContentIllustration1, 0, Qt::AlignCenter);

    layout->addStretch();
    m_contentWidget->setLayout(layout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
    switch(scrollLevel){
    case 0:
        m_scrollArea->verticalScrollBar()->setValue(0);
        break;
    case 1:
        m_scrollArea->verticalScrollBar()->setValue(timelineButtonsTitle->y());
        break;
    case 2:
        m_scrollArea->verticalScrollBar()->setValue(shotInfosTitle->y());
        break;
    case 3:
        m_scrollArea->verticalScrollBar()->setValue(exportationTitle->y());
    }
}


void HelperWidget::animateWidget(QWidget* widgetToAnimate, const bool state)
{
    auto *anim = new QPropertyAnimation(widgetToAnimate, "maximumHeight");
    anim->setDuration(200);

    if (state)
    {
        widgetToAnimate->show();

        anim->setStartValue(0);
        anim->setEndValue(widgetToAnimate->layout()->sizeHint().height());
    }
    else
    {
        anim->setStartValue(widgetToAnimate->height());
        anim->setEndValue(0);

        QObject::connect(anim, &QPropertyAnimation::finished,
                         widgetToAnimate, &QWidget::hide);
    }

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void HelperWidget::addButtonDescription(QGridLayout* gridLayout, const QString& iconName, const QString& buttonLabel, const QString& buttonDescription, const unsigned int lineIndex)
{
    QLabel* extendedIcon = new QLabel();
    extendedIcon->setPixmap(QPixmap(iconName).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    extendedIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel* extendedLabel = new QLabel("<b>" + buttonLabel + "</b>");
    extendedLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLabel* extendedDescription = new QLabel(buttonDescription);
    //extendedDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    extendedDescription->setWordWrap(true);
    extendedDescription->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    gridLayout->addWidget(extendedIcon, lineIndex, 0, Qt::AlignCenter);
    gridLayout->addWidget(extendedLabel, lineIndex, 1, Qt::AlignCenter);
    gridLayout->addWidget(extendedDescription, lineIndex, 2, Qt::AlignLeft);
}
