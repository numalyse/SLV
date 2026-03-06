#include "PlaylistItem.h"
#include <qevent.h>

PlaylistItem::PlaylistItem(QWidget *parent, const QString &mediaFilePath)
    : QWidget{parent}
{
    m_mediaData = new Media(mediaFilePath);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    m_indexLabel = new QLabel("0");
    mainLayout->addWidget(m_indexLabel);
    m_mediaThumbnailLabel = new QLabel("Miniature indisponible");
    mainLayout->addWidget(m_mediaThumbnailLabel);

    // infos titre, durée et type
    QVBoxLayout *mediaInfoLayout = new QVBoxLayout();
    m_mediaTitleLabel = new QLabel(m_mediaData->fileName());
    mediaInfoLayout->addWidget(m_mediaTitleLabel);
    QHBoxLayout *mediaDurationLayout = new QHBoxLayout();
    m_mediaDurationLabel = new QLabel("00:00:00");
    mediaInfoLayout->addWidget(m_mediaDurationLabel);
    m_mediaTypeIcon = new QLabel();
    m_mediaThumbnailImage = new QPixmap("speed.png");
    m_mediaTypeIcon->setPixmap(*m_mediaThumbnailImage);
    mediaInfoLayout->addWidget(m_mediaTypeIcon);
    mediaInfoLayout->addLayout(mediaDurationLayout);
    mainLayout->addLayout(mediaInfoLayout);
    m_deleteBtn = new QPushButton("X");
    mainLayout->addWidget(m_deleteBtn);
    initStyle();

    connect(m_mediaData, &Media::durationParsed, this, &PlaylistItem::setDurationLabel);
    connect(m_mediaData, &Media::fpsParsed, this, &PlaylistItem::computeThumbnail);
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]{ deleteItemRequested(m_itemIndex); });
}

void PlaylistItem::setDurationLabel()
{
    QString time = TimeFormatter::msToHHMMSSFF(m_mediaData->duration(), 1);
    m_mediaDurationLabel->setText(time);
}

void PlaylistItem::setIndex(int index)
{
    m_itemIndex = index;
    m_indexLabel->setText(QString::number(m_itemIndex));
}

void PlaylistItem::computeThumbnail()
{
    // recupérer frame à 1/15 de la vidéo à partir de duration et fps avec opencv get frame
}

// --- EVENTS --- //

void PlaylistItem::enterEvent(QEnterEvent *event)
{
    setStyleSheet("PlaylistItem{border-style: solid; border-color: black; border-radius: 3px; border-width: 2px;}");
    // QWidget::enterEvent(event);
}

void PlaylistItem::leaveEvent(QEvent *event)
{
    setStyleSheet("PlaylistItem{border-style: solid; border-color: black; border-radius: 3px; border-width: 1px;}");
    m_isClicked = false;
    // QWidget::leaveEvent(event);
}

void PlaylistItem::mousePressEvent(QMouseEvent *event)
{
    m_isClicked = true; // pas besoin de vérifier si bouton delete cliqué car le bouton bypass l'event de PlaylistItem
}

void PlaylistItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_isClicked && rect().contains(event->pos())){
        emit playPlaylistItemRequested(m_mediaData->filePath());
    }
}

void PlaylistItem::initStyle()
{
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("PlaylistItem{border-style: solid; border-color: black; border-radius: 3px; border-width: 1px;}");
    m_indexLabel->setMaximumWidth(15);
    m_mediaThumbnailLabel->setStyleSheet("background: lightgrey;");
    m_deleteBtn->setStyleSheet("background: tomato");
    m_deleteBtn->setMaximumWidth(20);
}
