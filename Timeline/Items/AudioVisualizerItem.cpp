#include "AudioVisualizerItem.h"
#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsView>

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

    // Downsampling : 1 colonne par pixel max
    int pixelWidth = static_cast<int>(m_width);
    int nAmps = m_amplitudes.size();
    if (nAmps == 0 || pixelWidth <= 0) return;

    QGraphicsView* view = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if (!view) return;

    // zone visible dans la scène
    QRectF visibleSceneRect = view->mapToScene(view->viewport()->rect()).boundingRect();
    QRectF visibleItemRect = mapFromScene(visibleSceneRect).boundingRect();

    if (nAmps <= pixelWidth) {
        // Cas classique : moins d'amplitudes que de pixels
        for (int IAmp = 0; IAmp < nAmps; ++IAmp) {
            double x = IAmp * m_columnWidth;
            if(x + m_columnWidth < visibleItemRect.left() || x > visibleItemRect.right())
                continue;
            double normAmp = (m_amplitudes[IAmp] - m_minAmplitude) * m_sizeCoeff;
            p->drawRect(IAmp * m_columnWidth, m_topMargin + m_height, m_columnWidth, -normAmp);
        }
    } else {
        // Downsampling : on regroupe les amplitudes par bloc
        double blockSize = static_cast<double>(nAmps) / pixelWidth;
        for (int i = 0; i < pixelWidth; ++i) {
            double x = i * (m_width / pixelWidth);
            double w = m_width / pixelWidth;
            if(x + w < visibleItemRect.left() || x > visibleItemRect.right())
                continue;
            int start = static_cast<int>(i * blockSize);
            int end = static_cast<int>((i + 1) * blockSize);
            if (end > nAmps) end = nAmps;
            double maxAmp = m_amplitudes[start];
            for (int j = start + 1; j < end; ++j) {
                if (m_amplitudes[j] > maxAmp) maxAmp = m_amplitudes[j];
            }
            double normAmp = (maxAmp - m_minAmplitude) * m_sizeCoeff;
            p->drawRect(x, m_topMargin + m_height, w, -normAmp);
        }
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
