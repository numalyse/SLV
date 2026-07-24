#include "AudioVisualizerItem.h"
#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsView>

AudioVisualizerItem::AudioVisualizerItem(const QVector<double>& amplitudes, double width, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_width{width}
{
    setZValue(1);
    if (!amplitudes.isEmpty())
        m_columnWidth = width / amplitudes.size();
    else
        m_columnWidth = width;

    // Compute min and max for graphic items scaling
    double min = 0.0, max = 1.0;
    if (!amplitudes.isEmpty()) {
        min = *std::min_element(amplitudes.begin(), amplitudes.end());
        max = *std::max_element(amplitudes.begin(), amplitudes.end());
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

    // Convert to short ints to fill the class amplitudes list
    for (int IAmp = 0; IAmp < amplitudes.size(); ++IAmp) {
        m_amplitudes.append((amplitudes[IAmp] - m_minAmplitude) * m_sizeCoeff);
    }
}

QRectF AudioVisualizerItem::boundingRect() const
{
    return QRectF(0, s_topMargin, m_width, s_height);
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

        QPolygon poly;
        bool first = true;
        unsigned int last = 0; // index of the last visible amplitude
        for (int IAmp = 0; IAmp < nAmps; ++IAmp) {
            double x = IAmp * m_columnWidth;
            // if the amps are outside the visible window, skip
            if(x + m_columnWidth < visibleItemRect.left() || x > visibleItemRect.right())
                continue;

            // First value : we start with previous amp and its base
            if(first){
                first = false;
                if(IAmp == 0){
                    poly << QPoint(0, s_topMargin + s_height)
                         << QPoint(0, s_topMargin + s_height - m_amplitudes[IAmp]);
                }
                else{
                    double prevX = IAmp-1 * m_columnWidth;
                    poly << QPoint(prevX, s_topMargin + s_height)// base
                         << QPoint(prevX, s_topMargin + s_height - m_amplitudes[IAmp-1]);
                }
            }
            else{
                last = IAmp;
                poly << QPoint(IAmp * m_columnWidth, s_topMargin + s_height - m_amplitudes[IAmp]); // place amplitude point
            }

        }
        unsigned int indexLast = last >= m_amplitudes.size() ? m_amplitudes.size()-1 : last+1;
        double nextX = indexLast * m_columnWidth;
        poly << QPoint(nextX, s_topMargin + s_height - m_amplitudes[indexLast])
             << QPoint(nextX, s_topMargin + s_height); // base

        p->drawPolygon(poly);
    } else {

        // Downsampling : if there is more amplitudes than pixels, take max amp
        const double blockSize = static_cast<double>(nAmps) / pixelWidth;
        QPolygon poly;
        bool first = true;
        unsigned int last = 0; // index of the last visible amplitude
        double lastAmp = 0.0;
        for (int IAmp = 0; IAmp < pixelWidth; ++IAmp) {
            m_columnWidth = m_width / pixelWidth;
            const double x = IAmp * m_columnWidth;
            // if the amps are outside the visible window, skip
            if(x + m_columnWidth < visibleItemRect.left() || x > visibleItemRect.right())
                continue;

            // Take the max value of the block
            int start = static_cast<int>(IAmp * blockSize);
            int end = static_cast<int>((IAmp + 1) * blockSize);
            if (end > nAmps) end = nAmps;
            double maxAmp = m_amplitudes[start];
            for (int j = start + 1; j < end; ++j) {
                if (m_amplitudes[j] > maxAmp) maxAmp = m_amplitudes[j];
            }
            if(first){ // Draw first visible amplitude with the previous one if it exists
                first = false;
                if(IAmp == 0){
                    poly << QPoint(0, s_topMargin + s_height) // base
                         << QPoint(0, s_topMargin + s_height - maxAmp);
                }
                else{
                    const double prevX = (IAmp-1) * m_columnWidth;
                    poly << QPoint(prevX, s_topMargin + s_height) // base
                         << QPoint(prevX, s_topMargin + s_height - m_amplitudes[IAmp-1]);
                }
            }
            else{
                last = IAmp;
                poly << QPoint(IAmp * m_columnWidth, s_topMargin + s_height - maxAmp); // place amplitude point
            }
        }
        const unsigned int indexLast = last >= pixelWidth ? pixelWidth-1 : last+1; // Draw last visible amplitude with the next ont if it exists
        const double nextX = indexLast * (m_width / pixelWidth);
        poly << QPoint(nextX, s_topMargin + s_height - lastAmp)
             << QPoint(nextX, s_topMargin + s_height); // base

        p->drawPolygon(poly);
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
