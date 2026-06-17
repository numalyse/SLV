#include "MediaLogoWidget.h"

#include <QHBoxLayout>

MediaLogoWidget::MediaLogoWidget(QWidget *parent, const QString &iconPath, int maxWidth)
: QWidget(parent)
{
    // pas besoin d'etre transparent donc on peut ne pas les mettre en fenetre flottante
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout* layout = new QHBoxLayout(this);
    m_logo = new AspectRatioPixmapLabel(this);
    m_logo->setMaximumWidth(maxWidth);

    QPixmap audioLogo(iconPath); 
    m_logo->setPixmap(audioLogo);
    m_logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_logo);
}

void MediaLogoWidget::onMediaRectChanged(const QRect &rect)
{
    if (!m_isShown)
        return;

    m_mediaRect = rect;
    update();
}

void MediaLogoWidget::paintEvent(QPaintEvent *event)
{
    if (!m_isShown)
        return;

    QWidget::paintEvent(event);
}

void MediaLogoWidget::setDisplay(bool isShown){
    m_isShown = isShown;
    if(m_isShown){
        update();
        show();
    } else {
        hide();
    }
}