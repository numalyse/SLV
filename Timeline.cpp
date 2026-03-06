#include "Timeline.h"
#include "ProjectManager.h"

#include <QVBoxLayout>
#include <QPainter>

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    
    m_ruler = new TimelineRuler(this);
    m_ruler->setAttribute(Qt::WA_StyledBackground, true);
    m_ruler->setStyleSheet(        
        "QWidget {"
        "   background-color: yellow;"
        "}");

    m_videoTrack = new TimelineVideoTrack(this);
    m_videoTrack->setAttribute(Qt::WA_StyledBackground, true);
    m_videoTrack->setStyleSheet(        
        "QWidget {"
        "   background-color: green;"
        "}");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_ruler);
    mainLayout->addWidget(m_videoTrack);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

}

void Timeline::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int64_t duration = ProjectManager::instance().projet()->media->duration();
    double ratio = duration / width();
    double fps = ProjectManager::instance().projet()->media->fps();

    


    p.end();
}