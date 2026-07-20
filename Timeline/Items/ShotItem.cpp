#include "ShotItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QPen>

ShotItem::ShotItem(Shot shot, double width, QGraphicsItem* parent)
: QGraphicsItem(parent), m_shot{shot}, m_width{width}, m_baseColor{m_shot.color}
{
    setZValue(0);

 
    // to keep the shotItem undearneath the audio visualizer but to have our "selected m_selectedNumber above the visualizer"
    // we create an item that will follow shotItem but with a different z value 
    m_selectionBox = new QGraphicsRectItem();     // it can't have this as parent, it would place it below the audio visualizer
    m_selectionBox->setZValue(4); 

    m_selectionBox->setPen(QPen(Qt::white, 2)); 
    m_selectionBox->setBrush(Qt::NoBrush); 

    m_selectionText = new QGraphicsTextItem(m_selectionBox);
    QFont textFont; 
    textFont.setPointSize(16);
    textFont.setBold(true); 
    m_selectionText->setFont(textFont);

    m_selectionBox->setVisible(false);
}

ShotItem::~ShotItem()
{
    delete m_selectionBox;
}

QColor ShotItem::getTagImageColor(const QPixmap &pixmap) const
{
    if (pixmap.isNull())
        return QColor();

    QImage image = pixmap.toImage();

    if (image.isNull())
        return QColor();

    // Parameters for the K-means algorithm
    constexpr int BLACK_THRESHOLD = 25;
    constexpr int NB_STEP = 32;
    constexpr int K = 5;
    constexpr int ITERATIONS = 10;

    const int width = image.width();
    const int height = image.height();

    // Check for black borders (horizontal and vertical) and crop them

    auto isBlackRow = [&](int y)
    {
        int black = 0;
        int samples = 0;

        for (int x = 0; x < width; x += 4)
        {
            QColor c = image.pixelColor(x, y);

            if (c.red() < BLACK_THRESHOLD &&
                c.green() < BLACK_THRESHOLD &&
                c.blue() < BLACK_THRESHOLD)
            {
                black++;
            }

            samples++;
        }

        return black > samples * 0.8;
    };

    auto isBlackColumn = [&](int x)
    {
        int black = 0;
        int samples = 0;

        for (int y = 0; y < height; y += 4)
        {
            QColor c = image.pixelColor(x, y);

            if (c.red() < BLACK_THRESHOLD &&
                c.green() < BLACK_THRESHOLD &&
                c.blue() < BLACK_THRESHOLD)
            {
                black++;
            }

            samples++;
        }

        return black > samples * 0.8;
    };

    int cropTop = 0;
    while (cropTop < height / 4 && isBlackRow(cropTop))
        cropTop++;

    int cropBottom = height - 1;
    while (cropBottom > height * 3 / 4 && isBlackRow(cropBottom))
        cropBottom--;

    if (cropBottom - cropTop < height / 2)
    {
        cropTop = 0;
        cropBottom = height - 1;
    }

    int cropLeft = 0;
    while (cropLeft < width / 4 && isBlackColumn(cropLeft))
        cropLeft++;

    int cropRight = width - 1;
    while (cropRight > width * 3 / 4 && isBlackColumn(cropRight))
        cropRight--;

    if (cropRight - cropLeft < width / 2)
    {
        cropLeft = 0;
        cropRight = width - 1;
    }

    const int usableWidth = cropRight - cropLeft + 1;
    const int usableHeight = cropBottom - cropTop + 1;

    const int stepX = qMax(1, usableWidth / NB_STEP);
    const int stepY = qMax(1, usableHeight / NB_STEP);

    QVector<QColor> samples;

    for (int y = cropTop; y <= cropBottom; y += stepY)
    {
        for (int x = cropLeft; x <= cropRight; x += stepX)
        {
            samples.push_back(image.pixelColor(x, y));
        }
    }

    if (samples.isEmpty())
        return QColor();

    // K-means clustering to find the dominant color

    QVector<QColor> centers;

    for (int i = 0; i < K; ++i)
        centers.push_back(samples[i * samples.size() / K]);

    QVector<int> assignment(samples.size());

    for (int iter = 0; iter < ITERATIONS; ++iter)
    {
        for (int i = 0; i < samples.size(); ++i)
        {
            int bestCluster = 0;
            int bestDistance = INT_MAX;

            for (int c = 0; c < K; ++c)
            {
                int dr = samples[i].red() - centers[c].red();
                int dg = samples[i].green() - centers[c].green();
                int db = samples[i].blue() - centers[c].blue();

                int dist = dr * dr + dg * dg + db * db;

                if (dist < bestDistance)
                {
                    bestDistance = dist;
                    bestCluster = c;
                }
            }

            assignment[i] = bestCluster;
        }

        QVector<int> count(K, 0);
        QVector<int> sumR(K, 0);
        QVector<int> sumG(K, 0);
        QVector<int> sumB(K, 0);

        for (int i = 0; i < samples.size(); ++i)
        {
            int c = assignment[i];

            count[c]++;
            sumR[c] += samples[i].red();
            sumG[c] += samples[i].green();
            sumB[c] += samples[i].blue();
        }

        for (int c = 0; c < K; ++c)
        {
            if (count[c] == 0)
                continue;

            centers[c] = QColor(
                sumR[c] / count[c],
                sumG[c] / count[c],
                sumB[c] / count[c]);
        }
    }

    // Find the cluster with the most samples

    QVector<int> count(K, 0);

    for (int c : assignment)
        count[c]++;

    int bestCluster = 0;

    for (int c = 1; c < K; ++c)
    {
        if (count[c] > count[bestCluster])
            bestCluster = c;
    }

    return centers[bestCluster];
}


void ShotItem::setThumbnail(const QPixmap& pixmap){
    m_pixmap = pixmap;
    update();
}

QRectF ShotItem::boundingRect() const
{
    return QRectF(0, topMargin(), m_width, height());
}

void ShotItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    p->setRenderHint(QPainter::Antialiasing, false);

    // set border style
    QPen borderPen(m_shot.borderColor);
    borderPen.setWidth(1);
    p->setPen(borderPen);
    QColor drawColor = m_selected ? m_shot.color.lighter(150) : m_shot.color;

    // draw rect 
    p->setBrush(QBrush(drawColor));
    p->drawRect(0, topMargin(), m_width, height());

    // draw thumbnail if set
    if(!m_pixmap.isNull() && m_width > s_minSizeForImage){
        double targetHeight = height() - 4;
        double scaleRatio = targetHeight / m_pixmap.height();
        double scaledImgWidth = m_pixmap.width() * scaleRatio;
        double finalDrawWidth = qMin(scaledImgWidth, m_width);

        QRectF target(2.0, topMargin()+2.0, finalDrawWidth, targetHeight);
        double sourceCropWidth = finalDrawWidth / scaleRatio;

        QRectF srcRect(0, 0, sourceCropWidth, m_pixmap.height());

        p->drawPixmap(target, m_pixmap, srcRect);
    }

}

void ShotItem::setWidth(double width)
{
    if (m_width == width) return;
    prepareGeometryChange(); 
    m_width = width;
    updateTextPosition();
    update();
}

void ShotItem::setSelected(bool state) { 

    if (!m_selectionBox) {
        qDebug() << "[SHOTITEM] setSelected : m_selectionBox null";
        return;
    }
    
    m_selected = state;

    if (m_selected) {
        if (this->scene() != nullptr && m_selectionBox->scene() != this->scene()) {
            this->scene()->addItem(m_selectionBox);
        }

        m_selectionText->setPlainText(QString::number(m_selectedNumber));
        m_selectionText->setDefaultTextColor(m_shot.borderColor);
        
        updateTextPosition();
        
        m_selectionBox->setVisible(true);
    } else {
        m_selectionBox->setVisible(false);
    }

    update();
}

void ShotItem::updateTextPosition(){
    m_selectionBox->setRect(0, 0, m_width, height());
    m_selectionBox->setPos(this->mapToScene(0, topMargin()));

    QRectF textRect = m_selectionText->boundingRect();
    double xPos = (m_width - textRect.width()) / 2.0;
    double yPos = (height() - textRect.height()) / 2.0;
    
    m_selectionText->setPos(xPos, yPos);
}