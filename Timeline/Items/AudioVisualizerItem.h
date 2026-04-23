#ifndef AUDIOVISUALIZERITEM_H
#define AUDIOVISUALIZERITEM_H

#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>

class AudioVisualizerItem : public QGraphicsItem
{

private:
    const QVector<double> m_amplitudes;
    double m_width;
    double m_height;
    double m_topMargin;
    const QColor m_color{QColor(64, 39, 117, 255)};
    double m_sizeCoeff{300};
    double m_columnWidth;
    double m_minAmplitude;

public:
    AudioVisualizerItem(const QVector<double>& amplitudes, double width, double height = 40, double topMargin = 73, QGraphicsItem* parent = nullptr);
    void setWidth(const double width);
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    double width() const { return m_width; }
    int type() const override { return SLV::TypeAudioVisualizerItem; }
};

#endif // AUDIOVISUALIZERITEM_H
