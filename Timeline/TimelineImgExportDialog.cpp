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

    m_widthSpin = new QSpinBox;
    m_widthSpin->setRange(480, 10000);
    m_widthSpin->setSingleStep(100);
    m_widthSpin->setSuffix(" px");

    m_currentViewCheck = new QCheckBox(PrefManager::instance().getText("timeline_current_view"));

    auto* form = new QFormLayout;
    form->addRow(PrefManager::instance().getText("filename") + " : ", pathRow);
    form->addRow(PrefManager::instance().getText("timeline_img_width") + " : ", m_widthSpin);
    form->addRow(m_currentViewCheck);

    auto* group = new QGroupBox(PrefManager::instance().getText("export_source_selection_txt"));
    auto* groupLayout = new QVBoxLayout(group);

    auto addCheck = [groupLayout](QCheckBox*& member, const QString& text) {
        member = new QCheckBox(text);
        groupLayout->addWidget(member);
    };
    addCheck(m_rulerCheck,      PrefManager::instance().getText("timeline_ruler"));
    addCheck(m_shotsCheck,      PrefManager::instance().getText("media_info_section_video"));
    addCheck(m_thumbnailsCheck, PrefManager::instance().getText("general_timeline_shot_image"));
    addCheck(m_audioCheck,      PrefManager::instance().getText("media_info_section_audio"));
    addCheck(m_annotCheck,      PrefManager::instance().getText("tooltip_annotation_button"));
    addCheck(m_abCheck,         PrefManager::instance().getText("tooltip_ab_markers"));
    addCheck(m_cursorCheck,     PrefManager::instance().getText("timeline_cursor"));

    m_shotBordersCheck = new QCheckBox(PrefManager::instance().getText("shot_borders_option"));
    m_transparentCheck = new QCheckBox(PrefManager::instance().getText("timeline_transparent_background_option"));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_okBtn = buttons->button(QDialogButtonBox::Ok);
    m_okBtn->setText(PrefManager::instance().getText("tooltip_export"));
    buttons->button(QDialogButtonBox::Cancel)->setText(PrefManager::instance().getText("cancel_action"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addWidget(group);
    mainLayout->addWidget(m_shotBordersCheck);
    mainLayout->addWidget(m_transparentCheck);
    mainLayout->addWidget(buttons);

    connect(m_shotsCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_audioCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_currentViewCheck, &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_pathEdit,         &QLineEdit::textChanged, this, &TimelineImgExportDialog::updateDependencies);

    setOptions(TimelineImgExportOptions{});  
}

void TimelineImgExportDialog::setOptions(const TimelineImgExportOptions& o)
{
    m_currentViewCheck->setChecked(o.currentViewOnly);
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

    const bool view = m_currentViewCheck->isChecked();
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

    o.currentViewOnly       = m_currentViewCheck->isChecked();
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