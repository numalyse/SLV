#include "AdjustmentsWidget.h"
#include "PrefManager.h"
#include <QLayout>

AdjustmentsWidget::AdjustmentsWidget(QWidget *parent, const float brightnessValue, const float contrastValue,
                                     const float saturationValue, const float hueValue, const float gammaValue)
    : QWidget{parent}
{
    createButtons();
    initUiLayout();
    m_brightnessAdjustment->setValue(brightnessValue * 100);
    m_contrastAdjustment->setValue(contrastValue * 100);
    m_saturationAdjustment->setValue(saturationValue * 100);
    m_hueAdjustment->setValue(hueValue * 10);
    // m_gammaAdjustment->setValue(gammaValue * 10);
}


void AdjustmentsWidget::createButtons()
{
    m_brightnessAdjustment = new QSlider(Qt::Horizontal, this);
    m_brightnessAdjustment->setRange(0, 200);
    m_contrastAdjustment = new QSlider(Qt::Horizontal, this);
    m_contrastAdjustment->setRange(0, 200);
    m_saturationAdjustment = new QSlider(Qt::Horizontal, this);
    m_saturationAdjustment->setRange(0, 200);
    m_hueAdjustment = new QSlider(Qt::Horizontal, this);
    m_hueAdjustment->setRange(-1800, 1800);
    // m_gammaAdjustment = new QSlider(Qt::Horizontal, this);
    // m_gammaAdjustment->setRange(0, 100);

    m_brightnessValueLabel = new QLabel();
    m_contrastValueLabel = new QLabel();
    m_saturationValueLabel = new QLabel();
    m_hueValueLabel = new QLabel("0");

    m_resetBtn = new QPushButton();

    connect(m_brightnessAdjustment, &QSlider::valueChanged, this, [this](int value){
        m_brightnessValueLabel->setText(QString::number(value));
        emit adjustmentChangeRequested(libvlc_adjust_Brightness, float(value) * 0.01);
    });
    connect(m_contrastAdjustment, &QSlider::valueChanged, this, [this](int value){
        m_contrastValueLabel->setText(QString::number(value));
        emit adjustmentChangeRequested(libvlc_adjust_Contrast, float(value) * 0.01);
    });
    connect(m_hueAdjustment, &QSlider::valueChanged, this, [this](int value){
        m_hueValueLabel->setText(QString::number(int(float(value)/10.0)));
        emit adjustmentChangeRequested(libvlc_adjust_Hue, float(value) * 0.1);
    });
    connect(m_saturationAdjustment, &QSlider::valueChanged, this, [this](int value){
        m_saturationValueLabel->setText(QString::number(value));
        emit adjustmentChangeRequested(libvlc_adjust_Saturation, float(value) * 0.01);
    });
    // connect(m_gammaAdjustment, &QSlider::valueChanged, this, [this](int value){ emit adjustmentChangeRequested(libvlc_adjust_Gamma, float(value) * 0.1); });
    connect(m_resetBtn, &QPushButton::released, this, &AdjustmentsWidget::resetAdjustments);
}

void AdjustmentsWidget::initUiLayout()
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "AdjustmentsWidget {"
        " background-color: palette(base);"
        " border-radius: 6px;"
        " border: 1px solid palette(button);"
        "}"
        "QPushButton {"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover {"
        "   background-color: palette(button);"
        "   border: 1px solid palette(button);"
        "   border-radius: 4px;"
        "}"
        );
    // setStyleSheet("background: red;");
    setContentsMargins(5,5,5,5);
    QGridLayout* slidersLayout = new QGridLayout(this);
    slidersLayout->setContentsMargins(0,0,0,0);
    slidersLayout->setSpacing(5);

    QLabel* brightnessLabel = new QLabel();
    brightnessLabel->setPixmap(QPixmap(":/icons/brightness_white").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    brightnessLabel->setToolTip(PrefManager::instance().getText("brightness"));
    slidersLayout->addWidget(brightnessLabel, 0, 0, Qt::AlignLeft);
    slidersLayout->addWidget(m_brightnessAdjustment, 0, 1);
    slidersLayout->addWidget(m_brightnessValueLabel, 0, 2, Qt::AlignRight);

    QLabel* contrastLabel = new QLabel();
    contrastLabel->setPixmap(QPixmap(":/icons/contrast_white").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    contrastLabel->setToolTip(PrefManager::instance().getText("contrast"));
    slidersLayout->addWidget(contrastLabel, 1, 0, Qt::AlignLeft);
    slidersLayout->addWidget(m_contrastAdjustment, 1, 1);
    slidersLayout->addWidget(m_contrastValueLabel, 1, 2, Qt::AlignRight);

    QLabel* saturationLabel = new QLabel();
    saturationLabel->setPixmap(QPixmap(":/icons/saturation_white").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    saturationLabel->setToolTip(PrefManager::instance().getText("saturation"));
    slidersLayout->addWidget(saturationLabel, 2, 0, Qt::AlignLeft);
    slidersLayout->addWidget(m_saturationAdjustment, 2, 1);
    slidersLayout->addWidget(m_saturationValueLabel, 2, 2, Qt::AlignRight);

    QLabel* hueLabel = new QLabel();
    hueLabel->setPixmap(QPixmap(":/icons/hue_white").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    hueLabel->setToolTip(PrefManager::instance().getText("hue"));
    slidersLayout->addWidget(hueLabel, 3, 0, Qt::AlignLeft);
    slidersLayout->addWidget(m_hueAdjustment, 3, 1);
    slidersLayout->addWidget(m_hueValueLabel, 3, 2, Qt::AlignRight);

    // Pour éviter que le changement de valeur bouge tout le popup
    QFontMetrics fm(m_hueValueLabel->font());
    int width = fm.horizontalAdvance("-180");
    m_hueValueLabel->setMinimumWidth(width);
    m_hueValueLabel->setAlignment(Qt::AlignRight);

    // Ne change rien à l'image
    // slidersLayout->addWidget(new QLabel(PrefManager::instance().getText("gamma")), 4, 0, Qt::AlignLeft);
    // slidersLayout->addWidget(m_gammaAdjustment, 4, 1);

    m_resetBtn->setIcon(QIcon(":/icons/reset_adjustments_white"));
    slidersLayout->addWidget(m_resetBtn, 5, 2, Qt::AlignRight);

    setLayout(slidersLayout);
}

void AdjustmentsWidget::resetAdjustments()
{
    m_brightnessAdjustment->setValue(100);
    m_contrastAdjustment->setValue(100);
    m_saturationAdjustment->setValue(100);
    m_hueAdjustment->setValue(0);
    // m_gammaAdjustment->setValue(10);
    emit resetAdjustmentsRequested();
}
