#ifndef AUDIOVISUALIZERITEM_H
#define AUDIOVISUALIZERITEM_H

#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>

class AudioVisualizerItem : public QGraphicsItem
{

private:
    const QVector<double> m_amplitudes;
    double m_width;
    const QColor m_color{QColor(108, 73, 184, 255)};
    double m_sizeCoeff{300};
    double m_columnWidth;
    double m_minAmplitude;

    static constexpr double s_height = 40.0;
    static constexpr double s_topMargin = 93.0;

public:
    AudioVisualizerItem(const QVector<double>& amplitudes, double width, QGraphicsItem* parent = nullptr);
    void setWidth(const double width);
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    double width() const { return m_width; }
    int type() const override { return SLV::TypeAudioVisualizerItem; }
};

#endif // AUDIOVISUALIZERITEM_H
