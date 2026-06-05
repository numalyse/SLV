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
    // setFixedSize(500, 400);
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

    m_classicTab = new CustomCheckbox("<b>" + pref.getText("help_menu_classic_checkbox") + "</b>");

    QWidget *classicSubTab = new QWidget();
    QVBoxLayout *classicSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *openFilesLabel = new CustomHoverLabel(pref.getText("help_menu_open_files_label"));
    CustomHoverLabel *classicToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_classic_toolbar_label"));
    CustomHoverLabel *extendedToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_extended_toolbar_label"));
    classicSubTabLayout->addWidget(openFilesLabel);
    classicSubTabLayout->addWidget(classicToolbarLabel);
    classicSubTabLayout->addWidget(extendedToolbarLabel);
    classicSubTab->setLayout(classicSubTabLayout);
    connect(m_classicTab, &CustomCheckbox::stateChanged, this, [this, classicSubTab](const bool state){
        animateWidget(classicSubTab, state);
    });

    m_multiviewTab = new CustomCheckbox("<b>" + pref.getText("help_menu_multiview_checkbox") + "</b>");

    QWidget *multiviewSubTab = new QWidget();
    QVBoxLayout *multiviewSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *enterMultiviewLabel = new CustomHoverLabel(pref.getText("help_menu_enter_multiview_label"));
    CustomHoverLabel *simplifiedToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_simplified_toolbar_label"));
    CustomHoverLabel *globalToolbarLabel = new CustomHoverLabel(pref.getText("help_menu_global_toolbar_label"));
    multiviewSubTabLayout->addWidget(enterMultiviewLabel);
    multiviewSubTabLayout->addWidget(simplifiedToolbarLabel);
    multiviewSubTabLayout->addWidget(globalToolbarLabel);
    multiviewSubTab->setLayout(multiviewSubTabLayout);
    connect(m_multiviewTab, &CustomCheckbox::stateChanged, this, [this, multiviewSubTab](const bool state){
        animateWidget(multiviewSubTab, state);
    });

    m_playlistTab = new CustomCheckbox("<b>" +pref.getText("help_menu_playlist_checkbox") + "</b>");

    QWidget *playlistSubTab = new QWidget();
    QVBoxLayout *playlistSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *accessPlaylistLabel = new CustomHoverLabel(pref.getText("help_menu_access_playlist_label"));
    CustomHoverLabel *addElementsPlaylistLabel = new CustomHoverLabel(pref.getText("help_menu_add_elements_playlist_label"));
    CustomHoverLabel *playlistButtonsLabel = new CustomHoverLabel(pref.getText("help_menu_playlist_buttons_label"));
    playlistSubTabLayout->addWidget(accessPlaylistLabel);
    playlistSubTabLayout->addWidget(addElementsPlaylistLabel);
    playlistSubTabLayout->addWidget(playlistButtonsLabel);
    playlistSubTab->setLayout(playlistSubTabLayout);
    connect(m_playlistTab, &CustomCheckbox::stateChanged, this, [this, playlistSubTab](const bool state){
        animateWidget(playlistSubTab, state);
    });

    m_timelineTab = new CustomCheckbox("<b>" +pref.getText("help_menu_timeline_checkbox") + "</b>");

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
    connect(m_timelineTab, &CustomCheckbox::stateChanged, this, [this, timelineSubTab](const bool state){
        animateWidget(timelineSubTab, state);
    });

    m_optionsTab = new CustomCheckbox("<b>" + pref.getText("help_menu_options_checkbox") + "</b>");

    QWidget *optionsSubTab = new QWidget();
    QVBoxLayout *optionsSubTabLayout = new QVBoxLayout();

    CustomHoverLabel *settingsLabel = new CustomHoverLabel(pref.getText("help_menu_settings_label"));
    CustomHoverLabel *shortcutsLabel = new CustomHoverLabel(pref.getText("help_menu_shortcuts_label"));
    CustomHoverLabel *pathsLabel = new CustomHoverLabel(pref.getText("help_menu_paths_label"));
    optionsSubTabLayout->addWidget(settingsLabel);
    optionsSubTabLayout->addWidget(shortcutsLabel);
    optionsSubTabLayout->addWidget(pathsLabel);
    optionsSubTab->setLayout(optionsSubTabLayout);
    connect(m_optionsTab, &CustomCheckbox::stateChanged, this, [this, optionsSubTab](const bool state){
        animateWidget(optionsSubTab, state);
    });

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

}

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


void HelperWidget::setClassicContent(int scrollLevel)
{
    PrefManager pref = PrefManager::instance();
    m_contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QLabel* classicContentTitle = new QLabel("<h1><b>" + pref.getText("help_menu_classic_checkbox") + "</b></h1>");

    QLabel* openFilesTitle = new QLabel("<h2><b>" + pref.getText("help_menu_open_files_label") + "</b></h2>");

    QLabel* openFilesContent1 = new QLabel(pref.getText("help_menu_open_files_content_1"));
    openFilesContent1->setWordWrap(true);

    QLabel* openFilesContentIllustration1 = new QLabel();
    QPixmap illustration1(":/help_dialog_illustrations/open_medias");
    openFilesContentIllustration1->setPixmap(illustration1.scaled(
        m_scrollArea->viewport()->width() - 30,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    // openFilesContentIllustration1->setScaledContents(true);

    QLabel* openFilesContent2 = new QLabel(pref.getText("help_menu_open_files_content_2"));
    openFilesContent1->setWordWrap(true);

    QLabel* openFilesContentIllustration2 = new QLabel();
    QPixmap illustration2(":/help_dialog_illustrations/open_medias_drag_drop");
    openFilesContentIllustration2->setPixmap(illustration2.scaled(
        m_scrollArea->viewport()->width() - 30,
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
    layout->addWidget(openFilesContentIllustration1);
    layout->addSpacing(20);
    layout->addWidget(openFilesContent2);
    layout->addSpacing(20);
    layout->addWidget(openFilesContentIllustration2);

    QLabel* classicToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_classic_toolbar_label") + "</b></h2>");

    QLabel* classicToolbarContent1 = new QLabel(pref.getText("help_menu_classic_toolbar_content_1"));

    QLabel* classicToolbarContentIllustration1 = new QLabel();
    QPixmap classicToolbarIllustration1(":/help_dialog_illustrations/monoview_toolbar_display");
    classicToolbarContentIllustration1->setPixmap(classicToolbarIllustration1.scaled(
        m_scrollArea->viewport()->width() - 30,
        10000,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    QLabel* classicToolbarButtonsLabel = new QLabel("<h3>" + pref.getText("help_menu_classic_toolbar_buttons_descriptions") + "</h3>");

    QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";
    QGridLayout* classicToolbarButtonsHelpLayout = new QGridLayout();
    classicToolbarButtonsHelpLayout->setSpacing(30);
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
    classicToolbarButtonsHelpLayout->setColumnStretch(2, 1);

    QLabel* stopIcon = new QLabel();
    stopIcon->setPixmap(QPixmap(":/icons/stop" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* stopLabel = new QLabel("<b>" + pref.getText("help_menu_stop_label") + "</b>");
    QLabel* stopDescription = new QLabel(pref.getText("help_menu_stop_description"));
    stopDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    stopDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addWidget(stopIcon, 1, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(stopLabel, 1, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(stopDescription, 1, 2, Qt::AlignLeft);

    QLabel* ejectIcon = new QLabel();
    ejectIcon->setPixmap(QPixmap(":/icons/eject" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* ejectLabel = new QLabel("<b>" + pref.getText("help_menu_eject_label") + "</b>");
    QLabel* ejectDescription = new QLabel(pref.getText("help_menu_eject_description"));
    ejectDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ejectDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addWidget(ejectIcon, 2, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(ejectLabel, 2, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(ejectDescription, 2, 2, Qt::AlignLeft);

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

    QLabel* speedIcon = new QLabel();
    speedIcon->setPixmap(QPixmap(":/icons/slow" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* speedLabel = new QLabel("<b>" + pref.getText("help_menu_speed_label") + "</b>");
    QLabel* speedDescription = new QLabel(pref.getText("help_menu_speed_description"));
    speedDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    speedDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addWidget(speedIcon, 4, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(speedLabel, 4, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(speedDescription, 4, 2, Qt::AlignLeft);

    QLabel* loopIcon = new QLabel();
    loopIcon->setPixmap(QPixmap(":/icons/loop_off" + theme).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* loopLabel = new QLabel("<b>" + pref.getText("help_menu_loop_label") + "</b>");
    QLabel* loopDescription = new QLabel(pref.getText("help_menu_loop_description"));
    loopDescription->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    loopDescription->setWordWrap(true);
    classicToolbarButtonsHelpLayout->addWidget(loopIcon, 5, 0, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(loopLabel, 5, 1, Qt::AlignCenter);
    classicToolbarButtonsHelpLayout->addWidget(loopDescription, 5, 2, Qt::AlignLeft);

    layout->addSpacing(10);
    layout->addWidget(classicToolbarTitle);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarContent1);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarContentIllustration1);
    layout->addSpacing(20);
    layout->addWidget(classicToolbarButtonsLabel);
    layout->addSpacing(10);
    layout->addLayout(classicToolbarButtonsHelpLayout);


    QLabel* extendedToolbarTitle = new QLabel("<h2><b>" + pref.getText("help_menu_extended_toolbar_label") + "</b></h2>");
    layout->addWidget(extendedToolbarTitle);

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
