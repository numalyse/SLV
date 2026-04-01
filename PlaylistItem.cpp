#include "PlaylistItem.h"
#include "TextManager.h"
#include <qevent.h>

PlaylistItem::PlaylistItem(QWidget *parent, const QString &mediaFilePath)
    : QWidget{parent}
{
    m_mediaData = new Media(mediaFilePath);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6,4,6,4);
    mainLayout->setSpacing(8);

    // index
    m_indexLabel = new QLabel("0");
    m_indexLabel->setFixedWidth(24);
    m_indexLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_indexLabel);

    // thumbnail
    m_mediaThumbnailLabel = new QLabel();
    m_mediaThumbnailLabel->setFixedSize(m_thumbnailSize);
    m_mediaThumbnailLabel->setText(TextManager::instance().get("No preview"));
    m_mediaThumbnailLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_mediaThumbnailLabel);

    // bloc info media
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);

    m_mediaTitleLabel = new QLabel(m_mediaData->fileName());
    m_mediaTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    infoLayout->addWidget(m_mediaTitleLabel);

    QHBoxLayout *metaLayout = new QHBoxLayout();

    m_mediaTypeIcon = new QLabel();
    m_mediaThumbnailImage = new QPixmap(":/icon/show_image_white");
    m_mediaTypeIcon->setPixmap(m_mediaThumbnailImage->scaled(16,16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_mediaDurationLabel = new QLabel("00:00:00");

    metaLayout->addWidget(m_mediaTypeIcon);
    metaLayout->addWidget(m_mediaDurationLabel);
    metaLayout->addStretch();

    infoLayout->addLayout(metaLayout);

    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();

    // bouton delete
    m_deleteBtn = new QPushButton("✕");
    m_deleteBtn->setFixedSize(24,24);
    mainLayout->addWidget(m_deleteBtn);

    initStyle();
    connect(m_mediaData, &Media::durationParsed, this, &PlaylistItem::setDurationLabel);
    connect(m_mediaData, &Media::fpsParsed, this, &PlaylistItem::computeThumbnail);
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]{ emit deleteItemRequested(m_itemIndex); });

    m_mediaData->parse();
}

void PlaylistItem::setDurationLabel()
{
    QString time = TimeFormatter::msToHHMMSSFF(m_mediaData->duration(), 1);
    m_mediaDurationLabel->setText(time);
    emit updateImageRequested(m_itemIndex, m_mediaData->duration()/2, 0, m_mediaData->filePath(), m_thumbnailSize);
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
    if(!m_isCurrentMedia)
        setStyleSheet("PlaylistItem{border-style: solid; border: 2px solid palette(button); border-radius: 3px;}");
    // QWidget::enterEvent(event);
}

void PlaylistItem::leaveEvent(QEvent *event)
{
    if(!m_isCurrentMedia)
        setStyleSheet("PlaylistItem{border-style: solid; border: 1px solid palette(button); border-radius: 3px;}");
    m_isClicked = false;
    // QWidget::leaveEvent(event);
}

void PlaylistItem::mousePressEvent(QMouseEvent *event)
{
    m_isClicked = true; // pas besoin de vérifier si bouton delete cliqué car le bouton bypass l'event de PlaylistItem
    m_dragStartPosition = event->pos();
}

void PlaylistItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_isClicked && rect().contains(event->pos())){
        emit updatePlaylistCurrentIndex(m_itemIndex);
        playMedia();
    }
}

void PlaylistItem::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("move-PlaylistItem", QByteArray::number(m_itemIndex));
    drag->setMimeData(mimeData);

    // Créer une pixmap pour le drag feedback
    QPixmap pixmap(size());
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());

    drag->exec(Qt::MoveAction);
}

void PlaylistItem::setCurrentMedia(bool isCurrent)
{
    m_isCurrentMedia = isCurrent;
    if(isCurrent)
        setStyleSheet("PlaylistItem{border-style: solid; border: 2px solid palette(light); border-radius: 3px;}");
    else
        setStyleSheet("PlaylistItem{border-style: solid; border: 1px solid palette(button); border-radius: 3px;}");
}

void PlaylistItem::playMedia()
{
    m_isCurrentMedia = true;
    emit playPlaylistItemRequested(m_mediaData->filePath());
}

void PlaylistItem::initStyle()
{
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("PlaylistItem{border-style: solid; border: 1px solid palette(button); border-radius: 3px;}");
    m_indexLabel->setMaximumWidth(15);
    m_mediaThumbnailLabel->setStyleSheet("background: palette(button);");
    m_deleteBtn->setStyleSheet("background: tomato");
    m_deleteBtn->setMaximumWidth(20);
}

void PlaylistItem::setThumbnail(QImage image)
{
    m_mediaThumbnailLabel->clear();
    QPixmap pixmap = QPixmap::fromImage(image);
    m_mediaThumbnailLabel->setPixmap(pixmap);
}
