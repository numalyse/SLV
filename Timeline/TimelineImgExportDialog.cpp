#include "TimelineImgExportDialog.h"

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
#include <QFileInfo>

TimelineImgExportDialog::TimelineImgExportDialog(const QString& defaultPath, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Exporter la timeline en PNG"));
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);

    auto* pathLayout = new QHBoxLayout();
    m_pathEdit = new QLineEdit(defaultPath, this);
    auto* browseBtn = new QPushButton(tr("Parcourir..."), this);
    pathLayout->addWidget(m_pathEdit, 1);
    pathLayout->addWidget(browseBtn);
    connect(browseBtn, &QPushButton::clicked, this, &TimelineImgExportDialog::browse);

    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(200, 32000);
    m_widthSpin->setSingleStep(100);
    m_widthSpin->setSuffix(" px");
    m_widthSpin->setValue(4000);

    m_currentViewCheck = new QCheckBox(tr("Exporter la vue actuelle (zoom et défilement courants)"), this);

    auto* form = new QFormLayout();
    form->addRow(tr("Fichier :"), pathLayout);
    form->addRow(tr("Largeur de l'image :"), m_widthSpin);
    mainLayout->addLayout(form);
    mainLayout->addWidget(m_currentViewCheck);

    auto* group = new QGroupBox(tr("Éléments à afficher"), this);
    auto* groupLayout = new QVBoxLayout(group);

    m_rulerCheck  = new QCheckBox(tr("Règle temporelle"), group);
    m_shotsCheck  = new QCheckBox(tr("Plans"), group);
    m_audioCheck  = new QCheckBox(tr("Forme d'onde audio"), group);
    m_annotCheck  = new QCheckBox(tr("Annotations"), group);
    m_abCheck     = new QCheckBox(tr("Marqueurs A/B"), group);
    m_cursorCheck = new QCheckBox(tr("Curseur de lecture"), group);

    for (QCheckBox* c : {m_rulerCheck, m_shotsCheck, m_audioCheck, m_annotCheck, m_abCheck})
        c->setChecked(true);
    m_cursorCheck->setChecked(false);

    for (QCheckBox* c : {m_rulerCheck, m_shotsCheck, m_audioCheck, m_annotCheck, m_abCheck, m_cursorCheck})
        groupLayout->addWidget(c);

    mainLayout->addWidget(group);

    m_transparentCheck = new QCheckBox(tr("Fond transparent"), this);
    mainLayout->addWidget(m_transparentCheck);

    connect(m_currentViewCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_widthSpin->setEnabled(!checked);
        m_transparentCheck->setEnabled(!checked);
        if (checked) m_transparentCheck->setChecked(false);
    });

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Exporter"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    auto updateOk = [this, buttons]() {
        buttons->button(QDialogButtonBox::Ok)->setEnabled(!m_pathEdit->text().trimmed().isEmpty());
    };
    connect(m_pathEdit, &QLineEdit::textChanged, this, updateOk);
    updateOk();
}

void TimelineImgExportDialog::browse()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"),
                                                m_pathEdit->text(), "PNG (*.png)");
    if (!path.isEmpty())
        m_pathEdit->setText(path);
}

TimelineImgExportOptions TimelineImgExportDialog::options() const
{
    TimelineImgExportOptions optionTL;
    optionTL.filePath = m_pathEdit->text().trimmed();
    if (QFileInfo(optionTL.filePath).suffix().isEmpty())
        optionTL.filePath += ".png";

    optionTL.width = m_widthSpin->value();
    optionTL.showRuler = m_rulerCheck->isChecked();
    optionTL.showShots = m_shotsCheck->isChecked();
    optionTL.showAudio = m_audioCheck->isChecked();
    optionTL.showAnnotations = m_annotCheck->isChecked();
    optionTL.showABMarkers = m_abCheck->isChecked();
    optionTL.showCursor = m_cursorCheck->isChecked();
    optionTL.transparentBackground = m_transparentCheck->isChecked();
    optionTL.currentViewOnly = m_currentViewCheck->isChecked();
    return optionTL;
}