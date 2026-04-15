#include "BlackOpacityWidget.h"
#include <QPainter>

BlackOpacityWidget::BlackOpacityWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    initSurface();
}

void BlackOpacityWidget::initSurface(){
    if (!m_surface) {
        m_surface = new QWidget(this);
        m_surface->setGeometry(m_mediaRect);
        m_surface->setAttribute(Qt::WA_TranslucentBackground);
        m_surface->setAttribute(Qt::WA_NoSystemBackground);
        m_surface->setAttribute(Qt::WA_OpaquePaintEvent, false);
        m_surface->hide();
    }
}

void BlackOpacityWidget::setBlackOpacityMode(bool isShown, double opacity){
    m_isShown = isShown;
    if(m_isShown){
        qDebug() << "BlackOpacityWidget - ON ";
        qDebug() << "BlackOpacityWidget - opacity received : " << opacity;
        m_opacityBlackFrame = std::clamp(opacity, 0.0, 1.0);
        update();

        m_surface->show();
        m_surface->raise();
    } else {
        qDebug() << "BlackOpacityWidget - OFF ";
        m_surface->hide();
    }
}

void BlackOpacityWidget::paintEvent(QPaintEvent *event)
{
    if (!m_isShown)
        return;

    if (m_opacityBlackFrame <= 0.0)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    m_blackColor.setAlphaF(m_opacityBlackFrame);

    p.fillRect(m_mediaRect, m_blackColor);

}

void BlackOpacityWidget::onMediaRectChanged(const QRect &rect)
{
    m_mediaRect = rect;
    qDebug() << "position : " << this->pos();
    qDebug() << "BlackOpacityWidget m_mediaRect : " << m_mediaRect;
    if(m_surface)
        m_surface->setGeometry(m_mediaRect);
    update();
}