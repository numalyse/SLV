#include "MediaLogoWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

MediaLogoWidget::MediaLogoWidget(QWidget *parent, const QString &iconPath, int maxWidth)
: QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    // stretch to center
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->addStretch();

    QHBoxLayout* logoRow = new QHBoxLayout();
    m_logo = new AspectRatioPixmapLabel(this);
    m_logo->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_logo->setAlignment(Qt::AlignCenter);
    logoRow->addWidget(m_logo);
    rootLayout->addLayout(logoRow);

    // layout to add optionnal widgets 
    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setAlignment(Qt::AlignCenter);
    rootLayout->addLayout(m_contentLayout);

    rootLayout->addStretch();

    setIcon(iconPath, maxWidth);
}

void MediaLogoWidget::setIcon(const QString &iconPath, int maxWidth)
{
    m_logo->setMaximumWidth(maxWidth);
    m_logo->setPixmap(QPixmap(iconPath));
}

void MediaLogoWidget::setContentVisible(bool visible)
{
    for (int i = 0; i < m_contentLayout->count(); ++i) {
        if (QWidget* w = m_contentLayout->itemAt(i)->widget())
            w->setVisible(visible);
    }
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