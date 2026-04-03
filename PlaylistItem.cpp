#include "PlaylistItem.h"
#include "TextManager.h"
#include <qevent.h>
#include <QBuffer>

PlaylistItem::PlaylistItem(QWidget *parent, const QString &mediaFilePath)
    : QWidget{parent}
{
    m_mediaData = new Media(mediaFilePath);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6,4,6,4);
    mainLayout->setSpacing(8);

    // n° index
    m_indexLabel = new QLabel("0");
    m_indexLabel->setFixedWidth(24);
    m_indexLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_indexLabel);

    // thumbnail
    m_mediaThumbnailLabel = new QLabel();
    m_mediaThumbnailLabel->setFixedSize(m_thumbnailSize);
    m_mediaThumbnailImage = new QPixmap(":/icons/hide_image_white");
    m_mediaThumbnailLabel->setPixmap(m_mediaThumbnailImage->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //m_mediaThumbnailLabel->setText(TextManager::instance().get("no_preview"));
    m_mediaThumbnailLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_mediaThumbnailLabel);

    // bloc info media
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);

    // titre
    m_mediaTitleLabel = new QLabel(m_mediaData->fileName());
    m_mediaTitleLabel->setToolTip(m_mediaData->fileName()+"."+m_mediaData->fileExtension());
    m_mediaTitleLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    infoLayout->addWidget(m_mediaTitleLabel);

    QHBoxLayout *metaLayout = new QHBoxLayout();

    // icone
    m_mediaTypeIconLabel = new QLabel();
    m_mediaTypeIcon = new QPixmap(":/icons/show_image_white");
    m_mediaTypeIconLabel->setPixmap(m_mediaTypeIcon->scaled(16,16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // durée
    m_mediaDurationLabel = new QLabel("00:00:00");

    metaLayout->addWidget(m_mediaTypeIconLabel);
    metaLayout->addWidget(m_mediaDurationLabel);

    infoLayout->addLayout(metaLayout);

    mainLayout->addLayout(infoLayout);
    mainLayout->addSpacing(10);

    // bouton delete
    m_deleteBtn = new QPushButton;
    m_deleteBtn->setIcon(QIcon(":/icons/delete_white"));
    m_deleteBtn->setToolTip(TextManager::instance().get("delete"));

    mainLayout->addWidget(m_deleteBtn);
    mainLayout->addSpacing(10);

    initStyle();
    connect(m_mediaData, &Media::durationParsed, this, &PlaylistItem::setDurationLabel);
    //connect(m_mediaData, &Media::fpsParsed, this, &PlaylistItem::computeThumbnail);
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]{ emit deleteItemRequested(m_itemIndex); });

    m_mediaData->parse();
    updateThumbnail();
}

void PlaylistItem::initStyle()
{
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("PlaylistItem{border-style: solid; border: 1px solid palette(button); border-radius: 3px;}");
    m_indexLabel->setMaximumWidth(15);
    m_mediaThumbnailLabel->setStyleSheet("background: palette(button);");

    m_deleteBtn->setFixedSize(24,24);
    m_deleteBtn->setMaximumWidth(20);
    m_deleteBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover{"
        "   background-color: tomato;"
        "   border: 1px solid palette(button);"
        "   border-radius: 4px;"
        "}");
}

void PlaylistItem::setDurationLabel()
{
    QString time = TimeFormatter::msToHHMMSSFF(m_mediaData->duration(), 1);
    QString timeChopped = time.left(time.length() - 3);
    m_mediaDurationLabel->setText(timeChopped);
    m_mediaDurationLabel->setToolTip(TextManager::instance().get("duration") + " : " + time);
    emit updateImageRequested(m_itemIndex, m_mediaData->duration()/2, 0, m_mediaData->filePath(), m_thumbnailSize);
}

void PlaylistItem::setIndex(int index)
{
    m_itemIndex = index;
    m_indexLabel->setText(QString::number(m_itemIndex+1));
}

void PlaylistItem::computeThumbnail() //il y a un setThumbnail
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
        setStyleSheet("PlaylistItem{border-style: solid; border: 2px solid palette(light); border-radius: 4px;}");
    else
        setStyleSheet("PlaylistItem{border-style: solid; border: 1px solid palette(button); border-radius: 4px;}");
}

void PlaylistItem::playMedia()
{
    m_isCurrentMedia = true;
    emit playPlaylistItemRequested(m_mediaData->filePath());
}

// void PlaylistItem::setThumbnail(QImage image)
// {
//     QPixmap pixmap = QPixmap::fromImage(image);
//     m_mediaThumbnailLabel->setPixmap(pixmap);
// }

void PlaylistItem::updateTypeIcon(){
    if (m_mediaData->type() != MediaType::Unknown)
        m_mediaTypeIconLabel->clear();

    if (m_mediaData->type() == MediaType::Video){
        m_mediaTypeIcon = new QPixmap(":/icons/music_note_white");
        m_mediaTypeIconLabel->setPixmap(m_mediaTypeIcon->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
        
    if (m_mediaData->type() == MediaType::Image){
        m_mediaTypeIcon = new QPixmap(":/icons/show_image_white");
        m_mediaTypeIconLabel->setPixmap(m_mediaTypeIcon->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
        
    if (m_mediaData->type() == MediaType::Audio){
        m_mediaTypeIcon = new QPixmap(":/icons/music_note_white");
        m_mediaTypeIconLabel->setPixmap(m_mediaTypeIcon->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void PlaylistItem::updateThumbnail()
{
    qDebug() << "type détecté : " << m_mediaData->type();
    if (m_mediaData->type() != MediaType::Unknown)
        m_mediaThumbnailLabel->clear();

    //QPixmap pixmap = QPixmap::fromImage(image);
    //m_mediaThumbnailLabel->setPixmap(pixmap);

    if (m_mediaData->type() == MediaType::Video){
        //QPixmap pixmap = QPixmap::fromImage(image);
        //m_mediaThumbnailLabel->setPixmap(pixmap);
    }
        

    if (m_mediaData->type() == MediaType::Image){
        QPixmap pixmap = QPixmap::fromImage(QImage(m_mediaData->filePath()));
        m_mediaThumbnailLabel->setPixmap(pixmap.scaled(thumbnailSize().width(), thumbnailSize().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (m_mediaData->type() == MediaType::Audio){
        m_mediaThumbnailImage = new QPixmap(":/icons/music_note_white");
        m_mediaThumbnailLabel->setPixmap(m_mediaThumbnailImage->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
        
}