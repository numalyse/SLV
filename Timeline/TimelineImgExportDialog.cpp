#include "Timeline/TimelineImgExportDialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "PrefManager.h"

TimelineImgExportDialog::TimelineImgExportDialog(const QString& defaultPath, QWidget* parent)
    : QDialog(parent)
{

    setWindowTitle(PrefManager::instance().getText("timeline_export_as_img"));
    setMinimumWidth(600);

    m_pathEdit = new QLineEdit(defaultPath);
    auto* browseBtn = new QPushButton(PrefManager::instance().getText("browse"));
    connect(browseBtn, &QPushButton::clicked, this, &TimelineImgExportDialog::browse);

    auto* pathRow = new QHBoxLayout;
    pathRow->addWidget(m_pathEdit, 1);
    pathRow->addWidget(browseBtn);
    
    auto* form = new QFormLayout;
    form->addRow(PrefManager::instance().getText("filename") + " : ", pathRow);
    
    // Radio buttons for view selection
    m_currentViewRadio = new QRadioButton(PrefManager::instance().getText("timeline_current_view"));
    m_currentViewRadio->setToolTip(PrefManager::instance().getText("timeline_current_view_tooltip"));
    m_fullViewRadio = new QRadioButton(PrefManager::instance().getText("timeline_full_view"));
    m_fullViewRadio->setToolTip(PrefManager::instance().getText("timeline_full_view_tooltip"));
    
    auto* viewGroup = new QButtonGroup(this);
    viewGroup->addButton(m_currentViewRadio);
    viewGroup->addButton(m_fullViewRadio);
    auto *radioLayout = new QHBoxLayout();
    radioLayout->addStretch();
    radioLayout->addWidget(m_currentViewRadio);
    radioLayout->addStretch();
    radioLayout->addWidget(m_fullViewRadio);
    radioLayout->addStretch();
    form->addRow(radioLayout);

    // // Width spin box
    // m_widthSpin = new QSpinBox;
    // m_widthSpin->setRange(480, 10000);
    // m_widthSpin->setSingleStep(100);
    // m_widthSpin->setSuffix(" px");
    // form->addRow(PrefManager::instance().getText("timeline_img_width") + " : ", m_widthSpin);

    auto* optionsGroup = new QGroupBox(PrefManager::instance().getText("export_source_selection_txt"));
    auto* optionsGeneralLayout = new QHBoxLayout(optionsGroup);
    auto* optionsCheckableLayout = new QVBoxLayout;
    optionsGeneralLayout->addLayout(optionsCheckableLayout);
    auto* previewLayout = new QVBoxLayout;
    optionsGeneralLayout->addLayout(previewLayout);

    auto addCheck = [optionsCheckableLayout](QCheckBox*& member, const QString& text) {
        member = new QCheckBox(text);
        optionsCheckableLayout->addWidget(member);
    };
    addCheck(m_rulerCheck,      PrefManager::instance().getText("timeline_ruler"));
    addCheck(m_annotCheck,      PrefManager::instance().getText("tooltip_annotation_button"));

    auto *shotsLayout = new QHBoxLayout;
    m_shotsCheck = new QCheckBox(PrefManager::instance().getText("media_info_section_video"));
    m_thumbnailsCheck = new QCheckBox(PrefManager::instance().getText("general_timeline_shot_image"));
    shotsLayout->addWidget(m_shotsCheck);
    shotsLayout->addWidget(m_thumbnailsCheck);
    shotsLayout->addStretch(); 
    optionsCheckableLayout->addLayout(shotsLayout);

    // addCheck(m_shotsCheck,      PrefManager::instance().getText("media_info_section_video"));
    // addCheck(m_thumbnailsCheck, PrefManager::instance().getText("general_timeline_shot_image"));

    addCheck(m_audioCheck,      PrefManager::instance().getText("media_info_section_audio"));
    addCheck(m_abCheck,         PrefManager::instance().getText("tooltip_ab_markers"));
    addCheck(m_cursorCheck,     PrefManager::instance().getText("timeline_cursor"));
    
    // Don't show these options for now, as it is not interesting for the user to export them
    // m_abCheck and m_cursorCheck are still available in the code, but not shown in the UI
    m_abCheck->hide();
    m_cursorCheck->hide();
    
    // Preview with elements that will be exported
    auto* previewLabel = new QLabel(PrefManager::instance().getText("timeline_img_preview_example"));
    previewLayout->addWidget(previewLabel);
    auto* previewScene = new QGraphicsScene(this);
    auto* previewView = new QGraphicsView(previewScene);
    previewView->setFixedSize(300, 200);
    previewLayout->addWidget(previewView);

    // Add a placeholder rectangle to represent the timeline preview
    previewScene->addRect(0, 0, 280, 180, QPen(Qt::black), QBrush(Qt::lightGray));

    



    // Shot borders and transparent background checkboxes
    m_shotBordersCheck = new QCheckBox(PrefManager::instance().getText("shot_borders_option"));
    m_transparentCheck = new QCheckBox(PrefManager::instance().getText("timeline_transparent_background_option"));

    auto* formBottom = new QFormLayout;

    // Width spin box
    m_widthSpin = new QSpinBox;
    m_widthSpin->setRange(480, 10000);
    m_widthSpin->setSingleStep(100);
    m_widthSpin->setSuffix(" px");
    formBottom->addRow(PrefManager::instance().getText("timeline_img_width") + " : ", m_widthSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_okBtn = buttons->button(QDialogButtonBox::Ok);
    m_okBtn->setText(PrefManager::instance().getText("tooltip_export"));
    buttons->button(QDialogButtonBox::Cancel)->setText(PrefManager::instance().getText("cancel_action"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(m_shotBordersCheck);
    mainLayout->addWidget(m_transparentCheck);
    mainLayout->addLayout(formBottom);
    mainLayout->addWidget(buttons);

    connect(m_shotsCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_audioCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_currentViewRadio, &QRadioButton::toggled,  this, &TimelineImgExportDialog::updateDependencies);
    connect(m_pathEdit,         &QLineEdit::textChanged, this, &TimelineImgExportDialog::updateDependencies);

    setOptions(TimelineImgExportOptions{});  
}

void TimelineImgExportDialog::setOptions(const TimelineImgExportOptions& o)
{
    m_currentViewRadio->setChecked(o.currentViewOnly);
    m_fullViewRadio->setChecked(!o.currentViewOnly);
    m_widthSpin->setValue(o.width);
    m_rulerCheck->setChecked(o.showRuler);
    m_annotCheck->setChecked(o.showAnnotations);
    m_shotsCheck->setChecked(o.showShots);
    m_thumbnailsCheck->setChecked(o.showThumbnails);
    m_shotBordersCheck->setChecked(o.showShotBorders);
    m_audioCheck->setChecked(o.showAudio);
    m_abCheck->setChecked(o.showABMarkers);
    m_cursorCheck->setChecked(o.showCursor);
    m_transparentCheck->setChecked(o.transparentBackground);
    updateDependencies();
}

void TimelineImgExportDialog::updateDependencies()
{
    const bool shots = m_shotsCheck->isChecked();
    const bool audio = m_audioCheck->isChecked();
    m_thumbnailsCheck->setEnabled(shots);
    m_shotBordersCheck->setEnabled(shots || audio);
    if (!shots)
        m_thumbnailsCheck->setChecked(false);

    const bool view = m_currentViewRadio->isChecked();
    m_widthSpin->setEnabled(!view);
    m_transparentCheck->setEnabled(!view);
    if (view)
        m_transparentCheck->setChecked(false);

    m_okBtn->setEnabled(!m_pathEdit->text().trimmed().isEmpty());
}

void TimelineImgExportDialog::browse()
{
    const QString path = QFileDialog::getSaveFileName(this, PrefManager::instance().getText("timeline_img_save"),
                                                      m_pathEdit->text(), "PNG (*.png)");
    if (!path.isEmpty())
        m_pathEdit->setText(path);
}

TimelineImgExportOptions TimelineImgExportDialog::options() const
{
    TimelineImgExportOptions o;
    o.filePath = m_pathEdit->text().trimmed();
    if (!o.filePath.endsWith(".png", Qt::CaseInsensitive))
        o.filePath += ".png";

    o.currentViewOnly       = m_currentViewRadio->isChecked();
    o.width                 = m_widthSpin->value();
    o.showRuler             = m_rulerCheck->isChecked();
    o.showAnnotations       = m_annotCheck->isChecked();
    o.showShots             = m_shotsCheck->isChecked();
    o.showThumbnails        = m_thumbnailsCheck->isChecked();
    o.showShotBorders       = m_shotBordersCheck->isChecked();
    o.showAudio             = m_audioCheck->isChecked();
    o.showABMarkers         = m_abCheck->isChecked();
    o.showCursor            = m_cursorCheck->isChecked();
    o.transparentBackground = m_transparentCheck->isChecked();
    return o;
}