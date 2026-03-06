#include "TimelineRuler.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"

#include <QPainter>

TimelineRuler::TimelineRuler(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(20);
}

void TimelineRuler::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(Qt::black, 3));
    p.drawLine(0, height(), width(), height()); 

    p.setPen(QPen(Qt::black, 1));

    int64_t duration = ProjectManager::instance().projet()->media->duration();
    double ratio = duration / width();
    double fps = ProjectManager::instance().projet()->media->fps();

    // dessin des traits
    for(int w = 100; w < width(); w += 100){
        p.drawLine(w, 0 + height()/2, w, height());
        int64_t currentTime = int64_t(ratio * w );
        QString txt = TimeFormatter::msToHHMMSSFF( currentTime , fps);
        p.drawText(w-(25), 2*height()/3, txt);
    }


    p.setPen(QPen(Qt::blue, 5));

    p.drawLine(250, 0, 250, height()); // on peut pas dessiner sur les widgets ruler et videotrack

    p.end();
}

