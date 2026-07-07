#include "TutoOpacityWidget.h"

#include <QPainter>

TutoOpacityWidget::TutoOpacityWidget(QWidget *parent, const QRect& windowRect) : BlackOpacityWidget(parent)
{
    m_targetRect = windowRect;
    initSurface();
}

TutoOpacityWidget::~TutoOpacityWidget()
{
}


void TutoOpacityWidget::followParentGeometry()
{
    if (QWidget *parent = qobject_cast<QWidget *>(parentWidget())) {
        setGeometry(parent->geometry());
        m_targetRect = rect();
        update();
    }
}

void TutoOpacityWidget::paintEvent(QPaintEvent *event)
{

    QPainter p(this);
    p.fillRect(rect(), QColor(0,0,0, 180)); 
    
    // trou pour le widget cible
    if(m_widgetToShow){
        QPoint topLeftGlobal = m_widgetToShow->mapToGlobal(QPoint(0, 0));
        QPoint topLeftLocal  = mapFromGlobal(topLeftGlobal);
        QRect hole(topLeftLocal, m_widgetToShow->size());
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(hole, Qt::transparent);
    }
}
