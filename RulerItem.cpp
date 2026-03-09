#include "RulerItem.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"

#include <QPainter>

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

    p->setRenderHint(QPainter::Antialiasing);

    p->setPen(QPen(Qt::black, 2));

    p->drawLine(0, m_height, m_width, m_height); 

    int64_t duration = ProjectManager::instance().projet()->media->duration();
    double ratio = duration / static_cast<float>(m_width);
    double fps = ProjectManager::instance().projet()->media->fps();


    for(int w = 100; w < m_width; w += 100){
        p->drawLine(w, 0 + m_height/4, w, m_height);
        int64_t currentTime = int64_t(ratio * w );
        QString txt = TimeFormatter::msToHHMMSSFF( currentTime , fps);
        p->drawText(w-(25), 2*m_height/3, txt);
    }
}


void RulerItem::setSize(int width, int height) {
    if (m_width == width && m_height == height) return;
    prepareGeometryChange(); 
    m_width = width;
    m_height = height;
    update();
}