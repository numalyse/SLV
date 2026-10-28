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
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QFrame>
#include <QFontMetricsF>
#include <QPixmap>
#include <QPainter>
#include <QPen>

#include "Annotation.h"
#include "PrefManager.h"
#include "Timeline/Items/AnnotationItem.h"
#include "Timeline/Items/AudioShotItem.h"
#include "Timeline/Items/AudioVisualizerItem.h"
#include "Timeline/Items/ShotItem.h"

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
    m_previewView = createPreview();
    previewLayout->addWidget(m_previewView);
    previewLayout->addStretch();

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
    connect(m_rulerCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_annotCheck,       &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_thumbnailsCheck,  &QCheckBox::toggled,     this, &TimelineImgExportDialog::updateDependencies);
    connect(m_currentViewRadio, &QRadioButton::toggled,  this, &TimelineImgExportDialog::updateDependencies);
    connect(m_pathEdit,         &QLineEdit::textChanged, this, &TimelineImgExportDialog::updateDependencies);

    setOptions(TimelineImgExportOptions{});  
}

QGraphicsView* TimelineImgExportDialog::createPreview()
{
    auto* previewView = new QGraphicsView;
    auto* previewScene = new QGraphicsScene(previewView);
    previewView->setScene(previewScene);
    previewView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    previewView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    previewView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    constexpr qreal previewWidth = 300.0;
    constexpr qreal rulerHeight = 24.0;
    const QColor rulerColor(181, 181, 181);

    auto* rulerLine = previewScene->addLine(0, rulerHeight, previewWidth, rulerHeight,
                                            QPen(rulerColor, 2));
        rulerLine->setZValue(1);
        m_previewRulerLine = rulerLine;

    const QFont rulerFont = previewView->font();
    QFontMetricsF rulerFontMetrics(rulerFont);
    auto* rulerStart = previewScene->addSimpleText("00:00:00.00", rulerFont);
    rulerStart->setBrush(rulerColor);
    rulerStart->setPos(2, 2);
    m_previewRulerStart = rulerStart;
    auto* rulerEnd = previewScene->addSimpleText("XX:XX:XX.XX", rulerFont);
    rulerEnd->setBrush(rulerColor);
    rulerEnd->setPos(previewWidth - rulerFontMetrics.horizontalAdvance(rulerEnd->text()) - 2,
                     2);
    m_previewRulerEnd = rulerEnd;

    Annotation sampleAnnotation{0, 0, tr("Annotation"), QString(), QColor(118, 1, 136), 0};
    auto* annotationItem = new AnnotationItem(sampleAnnotation, 140);
    annotationItem->setPos(45, -30);
    for (QGraphicsItem* handle : annotationItem->childItems())
        handle->setVisible(false);
    previewScene->addItem(annotationItem);
    m_previewAnnotation = annotationItem;

    Shot firstShot{};
    firstShot.color = QColor(79, 134, 198);
    firstShot.borderColor = QColor(33, 71, 115);
    firstShot.start = 0;
    firstShot.end = 200;

    Shot secondShot{};
    secondShot.color = QColor(79, 134, 198);
    secondShot.borderColor = QColor(33, 71, 115);
    secondShot.start = 200;
    secondShot.end = 300;

    auto* firstShotItem = new ShotItem(firstShot, firstShot.end - firstShot.start);
    firstShotItem->setPos(firstShot.start, -50);
    QPixmap thumbnail(80, 36);
    thumbnail.fill(QColor(125, 157, 184));
    {
        QPainter thumbnailPainter(&thumbnail);
        thumbnailPainter.setRenderHint(QPainter::Antialiasing, true);

        const QRect frame = thumbnail.rect().adjusted(1, 1, -1, -1);
        const QColor iconColor(33, 71, 115);

        thumbnailPainter.save();
        thumbnailPainter.setClipRect(frame);
        thumbnailPainter.setPen(Qt::NoPen);
        thumbnailPainter.setBrush(iconColor);
        thumbnailPainter.drawEllipse(QPointF(62.0, 11.0), 4.5, 4.5);
        thumbnailPainter.drawPolygon(QPolygonF{
            QPointF(0.0, 36.0), QPointF(0.0, 30.0), QPointF(20.0, 12.0),
            QPointF(35.0, 27.0), QPointF(52.0, 15.0), QPointF(80.0, 36.0)
        });
        thumbnailPainter.restore();

        thumbnailPainter.setPen(QPen(iconColor, 1));
        thumbnailPainter.setBrush(Qt::NoBrush);
        thumbnailPainter.drawRect(frame);
    }
    firstShotItem->setThumbnail(thumbnail);
    previewScene->addItem(firstShotItem);
    m_previewFirstShot = firstShotItem;

    auto* secondShotItem = new ShotItem(secondShot, secondShot.end - secondShot.start);
    secondShotItem->setPos(secondShot.start, -50);
    secondShotItem->setShowThumbnail(false);
    previewScene->addItem(secondShotItem);
    m_previewSecondShot = secondShotItem;

    AudioShot firstAudioShot;
    firstAudioShot.start = firstShot.start;
    firstAudioShot.end = firstShot.end;
    auto* firstAudioShotItem = new AudioShotItem(firstAudioShot, firstAudioShot.end - firstAudioShot.start);
    firstAudioShotItem->setPos(0, -93);
    firstAudioShotItem->setShowThumbnail(false);
    previewScene->addItem(firstAudioShotItem);
    m_previewFirstAudioShot = firstAudioShotItem;

    AudioShot secondAudioShot;
    secondAudioShot.start = secondShot.start;
    secondAudioShot.end = secondShot.end;
    auto* secondAudioShotItem = new AudioShotItem(secondAudioShot, secondAudioShot.end - secondAudioShot.start);
    secondAudioShotItem->setPos(secondShot.start, -93);
    secondAudioShotItem->setShowThumbnail(false);
    previewScene->addItem(secondAudioShotItem);
    m_previewSecondAudioShot = secondAudioShotItem;

    const QVector<double> amplitudeEnvelope{
        0.08, 0.09, 0.10, 0.10, 0.12, 0.11, 0.10, 0.09,
        0.10, 0.12, 0.13, 0.12, 0.11, 0.10, 0.11, 0.13,
        0.08, 0.09, 0.10, 0.10, 0.12, 0.11, 0.10, 0.09,
        0.22, 0.20, 0.26, 0.29, 0.28, 0.25, 0.21, 0.19,
        0.23, 0.27, 0.30, 0.28, 0.24, 0.21, 0.22, 0.26,
        0.28, 0.31, 0.33, 0.32, 0.35, 0.37, 0.40, 0.39,
        0.42, 0.44, 0.45, 0.44, 0.46, 0.49, 0.51, 0.54,
        0.55, 0.54, 0.52, 0.51, 0.53, 0.52, 0.58, 0.54,
        0.50, 0.45, 0.41, 0.37, 0.34, 0.31, 0.29, 0.27,
        0.26, 0.25, 0.24, 0.22, 0.20, 0.19, 0.18, 0.18,
        0.17, 0.19, 0.21, 0.20, 0.18, 0.17, 0.16, 0.17,
        0.19, 0.22, 0.24, 0.22, 0.20, 0.18, 0.17, 0.16,
        0.15, 0.15, 0.14, 0.14, 0.16, 0.22, 0.27, 0.29,
        0.28, 0.25, 0.21, 0.24, 0.28, 0.32, 0.30, 0.27,
        0.23, 0.20, 0.25, 0.29, 0.33, 0.37, 0.40, 0.42,
        0.39, 0.35, 0.31, 0.28, 0.26, 0.22, 0.19, 0.17,
        0.16, 0.15, 0.18, 0.21, 0.19, 0.16, 0.14, 0.15,
        0.17, 0.15, 0.13, 0.12, 0.14, 0.13, 0.11, 0.10,
        0.11, 0.12, 0.10, 0.09, 0.09, 0.10, 0.08, 0.08,
        0.09, 0.07, 0.07, 0.08, 0.06, 0.06, 0.05, 0.05,
        0.08, 0.09, 0.10, 0.10, 0.12, 0.11, 0.10, 0.09,
        0.11, 0.10, 0.12, 0.09, 0.13, 0.10, 0.14, 0.11,
        0.15, 0.15, 0.14, 0.14, 0.16, 0.22, 0.27, 0.25,
        0.21, 0.25, 0.29, 0.24, 0.28, 0.33, 0.27, 0.31,
        0.37, 0.30, 0.42, 0.35, 0.46, 0.39, 0.51, 0.43,
        0.56, 0.47, 0.62, 0.53, 0.68, 0.58, 0.72, 0.61,
        0.80, 0.84, 0.88, 0.92, 0.95, 0.91, 0.85, 0.78,
        0.70, 0.62, 0.54, 0.47, 0.41, 0.36, 0.40, 0.45,
        0.50, 0.52, 0.57, 0.44, 0.49, 0.38, 0.42, 0.31,
        0.35, 0.26, 0.29, 0.21, 0.24, 0.17, 0.20, 0.14,
        0.17, 0.12, 0.15, 0.11, 0.14, 0.10, 0.13, 0.11,
        0.15, 0.10, 0.12, 0.09, 0.13, 0.10, 0.14, 0.11,
        0.12, 0.09, 0.11, 0.08, 0.12, 0.10, 0.13, 0.09,
        0.11, 0.08, 0.12, 0.09, 0.14, 0.10, 0.12, 0.09,
        0.13, 0.10, 0.12, 0.11, 0.15, 0.11, 0.16, 0.12,
        0.17, 0.13, 0.20, 0.15, 0.24, 0.18, 0.28, 0.22,
        0.33, 0.26, 0.38, 0.31, 0.44, 0.36, 0.51, 0.42,
        0.58, 0.48, 0.63, 0.54, 0.69, 0.58, 0.66, 0.74,
        0.62, 0.70, 0.57, 0.68, 0.76, 0.64, 0.71, 0.59,
        0.66, 0.55, 0.62, 0.50, 0.57, 0.46, 0.52, 0.41,
        0.47, 0.37, 0.43, 0.33, 0.38, 0.29, 0.34, 0.26,
        0.30, 0.23, 0.27, 0.20, 0.24, 0.18, 0.21, 0.16,
        0.19, 0.14, 0.17, 0.12, 0.15, 0.11, 0.13, 0.09
        };
        QVector<double> sampleAmplitudes;
        sampleAmplitudes.reserve((amplitudeEnvelope.size() - 1) * 8 + 1);
        for (qsizetype i = 0; i < amplitudeEnvelope.size() - 1; ++i) {
            const double startAmplitude = amplitudeEnvelope[i];
            const double amplitudeChange = amplitudeEnvelope[i + 1] - startAmplitude;
            for (int step = 0; step < 8; ++step)
                sampleAmplitudes.append(startAmplitude + amplitudeChange * step / 8.0);
        }
        sampleAmplitudes.append(amplitudeEnvelope.last());

        auto* audioVisualizerItem = new AudioVisualizerItem(sampleAmplitudes, previewWidth);
        audioVisualizerItem->setPos(0, -93);
        previewScene->addItem(audioVisualizerItem);
        m_previewAudioVisualizer = audioVisualizerItem;
    
    return previewView;
}

void TimelineImgExportDialog::updatePreview()
{
    auto* previewScene = m_previewView->scene();

    constexpr qreal previewWidth = 300.0;
    constexpr qreal rulerHeight = 24.0;
    constexpr qreal rowSpacing = 4.0;

    qreal nextTrackTop = 0.0;
    if (m_rulerCheck->isChecked()) {
        m_previewRulerLine->setVisible(true);
        m_previewRulerStart->setVisible(true);
        m_previewRulerEnd->setVisible(true);
        m_previewRulerLine->setPos(0, nextTrackTop);
        m_previewRulerStart->setPos(2, nextTrackTop + 2);
        m_previewRulerEnd->setY(nextTrackTop + 2);
        nextTrackTop += rulerHeight + rowSpacing;
    } else {
        m_previewRulerLine->setVisible(false);
        m_previewRulerStart->setVisible(false);
        m_previewRulerEnd->setVisible(false);
    }

    const bool showAnnotations = m_annotCheck->isChecked();
    m_previewAnnotation->setVisible(showAnnotations);
    if (showAnnotations) {
        m_previewAnnotation->setPos(45, nextTrackTop - 30);
        nextTrackTop += AnnotationItem::height() + rowSpacing;
    }

    const bool showShots = m_shotsCheck->isChecked();
    m_previewFirstShot->setVisible(showShots);
    m_previewSecondShot->setVisible(showShots);
    m_previewFirstShot->setShowThumbnail(showShots && m_thumbnailsCheck->isChecked());
    m_previewSecondShot->setShowThumbnail(false);
    if (showShots) {
        m_previewFirstShot->setPos(0, nextTrackTop - 50);
        m_previewSecondShot->setPos(200, nextTrackTop - 50);
        nextTrackTop += 40.0 + rowSpacing;
    }

    const bool showAudio = m_audioCheck->isChecked();
    m_previewFirstAudioShot->setVisible(showAudio);
    m_previewSecondAudioShot->setVisible(showAudio);
    m_previewAudioVisualizer->setVisible(showAudio);
    if (showAudio) {
        m_previewFirstAudioShot->setPos(0, nextTrackTop - 93);
        m_previewSecondAudioShot->setPos(200, nextTrackTop - 93);
        m_previewAudioVisualizer->setPos(0, nextTrackTop - 93);
        nextTrackTop += 40.0 + rowSpacing;
    }

    const qreal sceneHeight = qMax(1.0, nextTrackTop - (nextTrackTop > 0.0 ? rowSpacing : 0.0));
    previewScene->setSceneRect(0, 0, previewWidth, sceneHeight);
    m_previewView->setFixedHeight(static_cast<int>(sceneHeight) + 2 * m_previewView->frameWidth());
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
    updatePreview();
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