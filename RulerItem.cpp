#include "RulerItem.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>

RulerItem::RulerItem(int width, int height, double minPxBetweenTicks, double pixelsPerMs, int64_t duration ,double fps ,QGraphicsItem* parent) 
: QGraphicsItem(parent), m_width{width}, m_height{height}, m_minPxBetweenTicks{minPxBetweenTicks}, m_pixelsPerMs{pixelsPerMs}, m_duration{duration}, m_fps{fps}
{
    setZValue(1);
    computeZoomSteps(m_fps, m_zoomSteps);
}

QRectF RulerItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

/// @brief Initialise les echelles de zoom
/// @param fps 
/// @param zoomSteps 
void RulerItem::computeZoomSteps(double fps, std::vector<double>& zoomSteps){
    zoomSteps.clear();

    double frameMs = (fps > 0) ? (1000.0 / fps) : 40.0; 
    
    zoomSteps.push_back(frameMs);
    zoomSteps.push_back(frameMs * 2);
    zoomSteps.push_back(frameMs * 5);
    zoomSteps.push_back(frameMs * 10);
    
    if (frameMs * 25.0 < 1000.0) {
        zoomSteps.push_back(frameMs * 25.0);
    }

    static const std::array<double, 11> fixedTimeStep = {
        1000.0, 2000.0, 5000.0, 10000.0, 30000.0, // Secondes
        60000.0, 120000.0, 300000.0, 600000.0,  // Minutes
        1800000.0, 3600000.0                // Heures
    };

    for (double t : fixedTimeStep) {
        zoomSteps.push_back(t);
    }
}

void RulerItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); 
    Q_UNUSED(widget);

    QRectF visibleZone;

    if (scene() && !scene()->views().isEmpty()) { // récupère la largeur du viewport et redessine dedans plutot que dans toute la graphics scene
        QGraphicsView *view = scene()->views().first();
        visibleZone = view->mapToScene(view->viewport()->rect()).boundingRect();
    }else {
        qDebug() << "Impossible de retrouver la zone visible";
    }

    Q_ASSERT(visibleZone.width() > 0);

    // dessin de la ligne horizontale du ruler
    p->setRenderHint(QPainter::Antialiasing);
    p->setPen(QPen(Qt::black, 2));
    p->drawLine(visibleZone.left(), m_height, visibleZone.right(), m_height); 


    double msPerPixels = 1.0 / m_pixelsPerMs;

    double minTimeStep = m_minPxBetweenTicks * msPerPixels;
    
    double maxZoomStep = m_zoomSteps[m_zoomSteps.size()-1];

    double stepMs = maxZoomStep; 
    for (double zoomStep : m_zoomSteps) {
        if (zoomStep >= minTimeStep) {
            stepMs = zoomStep;
            break;
        }
    }

    if (minTimeStep > maxZoomStep) {
        stepMs = (std::floor(minTimeStep / maxZoomStep) + 1.0) * maxZoomStep; 
    }

    double startTime = visibleZone.left() * msPerPixels;
    startTime = std::floor(startTime / stepMs) * stepMs; 
    startTime = ( startTime < 0) ? 0 : startTime;

    double endTime = visibleZone.right() * msPerPixels;

    // dessin des traits verticaux et du texte
    for(double t = startTime; t <= endTime; t += stepMs) {

        double px = t * m_pixelsPerMs;

         // on snap le temps retrouvé à un temps de la frametime la plus proche => évite d'avoir 2 timecode 00:00:00[00]
        double rawMs = px / m_pixelsPerMs;
        if (m_fps > 0) {
            double frameMs = 1000.0 / m_fps;
            double nearestFrameIndex = std::round(rawMs / frameMs);
            rawMs = nearestFrameIndex * frameMs; 
        } 
        int64_t finalMs = static_cast<int64_t>(rawMs);
        p->drawLine(QPointF(px, m_height / 1.3), QPointF(px, m_height));

        QString txt = TimeFormatter::msToHHMMSSFF(finalMs, m_fps);
        p->save();
        
        double textX = px - 25.0;
        textX = (textX < 0.0)  ? 2.0 : textX; // décale vers la droite le text de gauche

        p->translate(textX, 2.0 * m_height / 3.0);
        p->drawText(0.0, 0.0, txt);
        p->restore();
    }
}


void RulerItem::setSize(int width, int height, double pixelsPerMs) {
    if (m_width == width && m_height == height && m_pixelsPerMs == pixelsPerMs) return;
    prepareGeometryChange(); 
    m_width = width;
    m_height = height;
    m_pixelsPerMs = pixelsPerMs;
    update();
}