#include "RulerItem.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>

RulerItem::RulerItem(int width, int height, double minPxBetweenTicks, QGraphicsItem* parent) 
: QGraphicsItem(parent), m_width{width}, m_height{height}, m_minPxBetweenTicks{minPxBetweenTicks}
{
    setZValue(1);
}

QRectF RulerItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

/// @brief Modifie le cached fps avec le newFps, modifie le vecteur de zooms
/// @param newFps 
/// @param cachedFps 
/// @param zoomSteps 
void RulerItem::computeZoomSteps(double newFps, double& cachedFps, std::vector<int64_t>& zoomSteps){
    cachedFps = newFps;
    zoomSteps.clear();

    int64_t frameMs = (newFps > 0) ? static_cast<int64_t>(1000 / newFps) : 40; 
    
    zoomSteps.push_back(frameMs);
    zoomSteps.push_back(frameMs * 2);
    zoomSteps.push_back(frameMs * 5);
    zoomSteps.push_back(frameMs * 10);
    
    if (frameMs * 25 < 1000) {
        zoomSteps.push_back(frameMs * 25);
    }

    static const std::array<int64_t, 11> fixedTimeStep = {
        1000, 2000, 5000, 10000, 30000, // Secondes
        60000, 120000, 300000, 600000,  // Minutes
        1800000, 3600000                // Heures
    };

    for (int64_t t : fixedTimeStep) {
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

    int64_t duration = ProjectManager::instance().projet()->media->duration();
    double fps = ProjectManager::instance().projet()->media->fps();
    
    // ratio ms / px
    double msPerPixels = static_cast<double>(duration) / static_cast<double>(m_width);
    // ratio px / ms
    double pixelsPerMs = static_cast<double>(m_width) / static_cast<double>(duration);

    int64_t minTimeStep = static_cast<int64_t>(m_minPxBetweenTicks * msPerPixels);

    int64_t frameMs = (fps > 0) ? static_cast<int64_t>(1000 / fps) : 40; 
    
    // variables statiques recalculée quand on le media change de fps ou à l'initialisation
    // évite d'allouer un tableau à chaque appelle de paint
    static std::vector<int64_t> zoomSteps;
    static double cachedFps = -1.0;
    if (fps != cachedFps || zoomSteps.empty()) {
        computeZoomSteps(fps, cachedFps, zoomSteps);
    }
    int64_t maxZoomStep = zoomSteps[zoomSteps.size()-1];

    int64_t stepMs = maxZoomStep; 
    for (int64_t zoomStep : zoomSteps) {
        if (zoomStep >= minTimeStep) {
            stepMs = zoomStep;
            break;
        }
    }

    if (minTimeStep > maxZoomStep) {
        stepMs = ((minTimeStep / maxZoomStep) + 1) * maxZoomStep; 
    }

    int64_t startTime = static_cast<int64_t>(visibleZone.left() * msPerPixels);
    startTime = (startTime / stepMs) * stepMs; 
    int64_t endTime = static_cast<int64_t>(visibleZone.right() * msPerPixels);

    for(int64_t t = startTime; t <= endTime; t += stepMs) {
        
        int px = static_cast<int>(t * pixelsPerMs);

        p->drawLine(px, m_height/1.3, px, m_height);

        QString txt = TimeFormatter::msToHHMMSSFF(t, fps);
        p->save();
        p->translate(px - 25, 2 * m_height / 3);
        p->drawText(0, 0, txt);
        p->restore();
    }
}


void RulerItem::setSize(int width, int height) {
    if (m_width == width && m_height == height) return;
    prepareGeometryChange(); 
    m_width = width;
    m_height = height;
    update();
}