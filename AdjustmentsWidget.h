#ifndef ADJUSTMENTSWIDGET_H
#define ADJUSTMENTSWIDGET_H

#include "Timeline/ThumbnailWorker.h"

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <vlc/vlc.h>

class AdjustmentsWidget : public QWidget
{
    Q_OBJECT
public:

    /// @brief Widget including various sliders to adjust the current media colors and brightness
    explicit AdjustmentsWidget(QWidget *parent = nullptr, const float brightnessValue = 1.0, const float contrastValue = 1.0,
                               const float saturationValue = 1.0, const float hueValue = 0.0, const float gammaValue = 1.0);
    void createButtons();
    void initUiLayout();

public slots:
    void resetAdjustments();

private:
    QSlider* m_brightnessAdjustment;
    QSlider* m_contrastAdjustment;
    QSlider* m_saturationAdjustment;
    QSlider* m_hueAdjustment;
    // QSlider* m_gammaAdjustment;
    QPushButton* m_resetBtn;
    QLabel* m_brightnessValueLabel;
    QLabel* m_contrastValueLabel;
    QLabel* m_saturationValueLabel;
    QLabel* m_hueValueLabel;

signals:
    void adjustmentChangeRequested(const libvlc_video_adjust_option_t option, float value);
    void resetAdjustmentsRequested();
};

#endif // ADJUSTMENTSWIDGET_H
