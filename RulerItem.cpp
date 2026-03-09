#include "RulerItem.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>

RulerItem::RulerItem(int width, int height) : m_width{width}, m_height{height}
{
    setZValue(1);
}

QRectF RulerItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
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

    qDebug() << "Taille de la zone visible :" << visibleZone.width();

    p->setRenderHint(QPainter::Antialiasing);
    p->setPen(QPen(Qt::black, 2));

    p->drawLine(visibleZone.left(), m_height, visibleZone.right(), m_height); 

    int64_t duration = ProjectManager::instance().projet()->media->duration();
    double ratio = duration / static_cast<float>(m_width);
    double fps = ProjectManager::instance().projet()->media->fps();

    int startX = (visibleZone.left() < 0 ) ? 0 : visibleZone.left();
    int endX = visibleZone.right();

    for(int w = startX; w < endX; w += 100){
        p->drawLine(w, 0 + m_height/4, w, m_height);
        int64_t currentTime = int64_t(ratio * w );
        QString txt = TimeFormatter::msToHHMMSSFF( currentTime , fps);

        // si w est trop grand le texte ne se dessine plus à cause d'une limitation du systeme pour écrire
        p->save(); // On sauvegarde la position actuelle du pinceau
        p->translate(w - 25, 2 * m_height / 3); // On bouge le pinceau à l'endroit où on veut écrire
        p->drawText(0, 0, txt); // on dessine 
        p->restore(); // on se remet à la bonne position
    }
}


void RulerItem::setSize(int width, int height) {
    if (m_width == width && m_height == height) return;
    prepareGeometryChange(); 
    m_width = width;
    m_height = height;
    update();
}