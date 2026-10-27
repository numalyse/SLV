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

TimelineImgExportDialog::TimelineImgExportDialog(const QString& defaultPath, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Exporter la timeline en PNG"));

    m_pathEdit = new QLineEdit(defaultPath);
    auto* browseBtn = new QPushButton(tr("Parcourir..."));
    connect(browseBtn, &QPushButton::clicked, this, &TimelineImgExportDialog::browse);

    auto* pathRow = new QHBoxLayout;
    pathRow->addWidget(m_pathEdit, 1);
    pathRow->addWidget(browseBtn);

    m_widthSpin = new QSpinBox;
    m_widthSpin->setRange(480, 10000);
    m_widthSpin->setSingleStep(100);
    m_widthSpin->setSuffix(" px");

    m_currentViewCheck = new QCheckBox(tr("Exporter la vue actuelle (zoom et défilement courants)"));

    auto* form = new QFormLayout;
    form->addRow(tr("Fichier :"), pathRow);
    form->addRow(tr("Largeur de l'image :"), m_widthSpin);
    form->addRow(m_currentViewCheck);

    auto* group = new QGroupBox(tr("Éléments à afficher"));
    auto* groupLayout = new QVBoxLayout(group);

    auto addCheck = [groupLayout](QCheckBox*& member, const QString& text) {
        member = new QCheckBox(text);
        groupLayout->addWidget(member);
    };
    addCheck(m_rulerCheck,      tr("Règle temporelle"));
    addCheck(m_shotsCheck,      tr("Plans"));
    addCheck(m_thumbnailsCheck, tr("Imagettes des plans"));
    addCheck(m_audioCheck,      tr("Forme d'onde audio"));
    addCheck(m_annotCheck,      tr("Annotations"));
    addCheck(m_abCheck,         tr("Marqueurs A/B"));
    addCheck(m_cursorCheck,     tr("Curseur de lecture"));

    m_shotBordersCheck = new QCheckBox(tr("Contours des plans vidéo et audio"));
    m_transparentCheck = new QCheckBox(tr("Fond transparent"));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_okBtn = buttons->button(QDialogButtonBox::Ok);
    m_okBtn->setText(tr("Exporter"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Annuler"));
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
    const QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"),
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