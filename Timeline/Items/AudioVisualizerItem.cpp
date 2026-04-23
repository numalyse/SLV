#include "AudioVisualizerItem.h"
#include <QPainter>

AudioVisualizerItem::AudioVisualizerItem(const QVector<double>& amplitudes, double width, double height, double topMargin, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_amplitudes{amplitudes}, m_width{width}, m_height{height}, m_topMargin{topMargin}
{
    setZValue(1);
    if (!m_amplitudes.isEmpty())
        m_columnWidth = width / m_amplitudes.size();
    else
        m_columnWidth = width;

    // Calcul du min et max pour l'échelle
    double min = 0.0, max = 1.0;
    if (!m_amplitudes.isEmpty()) {
        min = *std::min_element(m_amplitudes.begin(), m_amplitudes.end());
        max = *std::max_element(m_amplitudes.begin(), m_amplitudes.end());
        if (max - min > 1e-6) {
            m_sizeCoeff = 40.0 / (max - min);
            m_minAmplitude = min;
        } else {
            m_sizeCoeff = 1.0;
            m_minAmplitude = 0.0;
        }
    } else {
        m_sizeCoeff = 1.0;
        m_minAmplitude = 0.0;
    }
}

QRectF AudioVisualizerItem::boundingRect() const
{
    return QRectF(0, m_topMargin, m_width, m_height);
}

void AudioVisualizerItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);
    p->setPen(m_color);
    p->setBrush(QBrush(m_color));

    for(size_t IAmp = 0; IAmp < m_amplitudes.size(); ++IAmp)
    {
        // Normalisation sur 40 pixels de haut
        double normAmp = (m_amplitudes[IAmp] - m_minAmplitude) * m_sizeCoeff;
        p->drawRect(IAmp * m_columnWidth, m_topMargin + m_height, m_columnWidth, -normAmp);
    }

}

void AudioVisualizerItem::setWidth(double width)
{
    if (m_width == width) return;
    prepareGeometryChange();
    m_width = width;
    if (!m_amplitudes.isEmpty())
        m_columnWidth = width / m_amplitudes.size();
    else
        m_columnWidth = width;
    update();
}
