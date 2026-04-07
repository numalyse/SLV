#include "Playlist.h"
#include "PrefManager.h"
#include "./ToolbarButtons/ToolbarButton.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDir>
#include <QDirIterator>

namespace {
QStringList collectValidFilesFromPath(const QString &path)
{
    const QStringList allowedExtensions = {"mp4", "avi", "mkv", "mov", "m4v", "vob", "png", "wav"};
    QStringList collected;

    QDir dir(path);
    if (!dir.exists())
        return collected;

    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QString ext = QFileInfo(filePath).suffix().toLower();
        if (allowedExtensions.contains(ext)) {
            collected.append(filePath);
        }
    }

    return collected;
}
}

Playlist::Playlist(QWidget *parent)
    : QWidget{parent}
{
    setAcceptDrops(true);
    m_mainLayout = new QVBoxLayout();
    this->setLayout(m_mainLayout);

    QHBoxLayout *playlistLabelLayout = new QHBoxLayout();
    m_mainLayout->addLayout(playlistLabelLayout);

    QLabel *playlistLabel = new QLabel();
    QFont font = playlistLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    playlistLabel->setFont(font);
    //playlistLabel->setTextFormat(Qt::RichText);
    playlistLabel->setText("<b>"+PrefManager::instance().getText("playlist")+"</b>");
    playlistLabelLayout->addWidget(playlistLabel); //ajouter les boutons random et loop peut-être


    //m_addItemBtn = new QPushButton("+");
    //m_addItemBtn = new ToolbarButton(this, "plus_white", "tooltip_add_item_playlist");
    //m_addItemBtn->setFixedSize(24,24);

    m_addItemBtn = new QPushButton;
    m_addItemBtn->setIcon(QIcon(":/icons/plus_white"));
    //m_addItemBtn->setIconSize(QSize(12,12));
    m_addItemBtn->setToolTip(PrefManager::instance().getText("tooltip_add_item_playlist"));
    m_addItemBtn->setFixedHeight(50);
    m_addItemBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: 2px dashed palette(button);"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover{"
        "   background-color: palette(button);"
        "   border: 2px solid palette(button);"
        "   border-radius: 4px;"
        "}");
    playlistLabelLayout->addWidget(m_addItemBtn);

    m_itemsLayout = new QVBoxLayout();
    m_mainLayout->addLayout(m_itemsLayout);
    m_mainLayout->addWidget(m_addItemBtn);
    m_mainLayout->addStretch();

    connect(m_addItemBtn, &ToolbarButton::clicked, this, &Playlist::addItemDialog);

}

void Playlist::updateThumbnail(int playlistItemId, QImage image)
{
    if(playlistItemId <0 || playlistItemId >= m_items.size()) return;

    auto* item = m_items[playlistItemId];
    item->setThumbnail(image);

}

void Playlist::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    qDebug() << "Playlist height : " << this->height();
}

void Playlist::dragEnterEvent(QDragEnterEvent *event){
    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("move-PlaylistItem")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Playlist::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasFormat("move-PlaylistItem")) {
        int draggedIndex = mimeData->data("move-PlaylistItem").toInt();

        int dropIndex = -1;
        for (int i = 0; i < m_items.size(); ++i) {
            QRect itemRect = m_items[i]->geometry();
            if (itemRect.contains(event->pos())) {
                dropIndex = i;
                break;
            }
        }

        if (dropIndex != -1 && dropIndex != draggedIndex) {
            PlaylistItem *draggedItem = m_items.takeAt(draggedIndex);
            m_items.insert(dropIndex, draggedItem);

            updateItemIndices();
            updateLayout();
        }

        event->acceptProposedAction();
    } else if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QStringList filePaths;

        for (const QUrl &url : urlList) {
            QString filePath = url.toLocalFile();
            qDebug() << "Fichier ou dossier droppé :" << filePath;

            QFileInfo info(filePath);
            if (info.isDir()) {
                filePaths.append(collectValidFilesFromPath(filePath));
            } else if (info.isFile()) {
                filePaths.append(filePath);
            }
        }

        if (!filePaths.isEmpty()) {
            addItemsFromPaths(filePaths);
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Playlist::addItemDialog()
{
    QStringList filesPaths = QFileDialog::getOpenFileNames(this, PrefManager::instance().getText("open_files"), "/", "Fichiers multimédia (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)");
    if(filesPaths.empty()){
        qDebug() << "PLAYLIST - Pas de fichier sélectionné";
        return;
    }
    addItemsFromPaths(filesPaths);

}

void Playlist::addItemsFromPaths(const QStringList &filesPaths)
{
    qDebug() << "PLAYLIST - Fichiers sélectionnés : " << filesPaths;
    for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
        PlaylistItem *newItem = new PlaylistItem(nullptr, filesPaths.at(IFilePath));
        newItem->setIndex(m_items.size());
        m_items.append(newItem);
        m_itemsLayout->addWidget(newItem);
        this->adjustSize();
        this->updateGeometry();
        
        connect(newItem, &PlaylistItem::deleteItemRequested, this, &Playlist::deleteItem);
        connect(newItem, &PlaylistItem::updatePlaylistCurrentIndex, this, [&](unsigned int index){
            m_currentMediaIndex = index;
        });
        connect(newItem, &PlaylistItem::updateImageRequested, this, &Playlist::updateImageRequested);
        connect(newItem, &PlaylistItem::playPlaylistItemRequested, this, &Playlist::playMedia);
        connect(&SignalManager::instance(), &SignalManager::mediaWidgetMediaFinished, this, &Playlist::playNextMedia);
    }
    if(!filesPaths.empty()){
        m_items[0]->playMedia();
    }
    emit disableToolbarLoopRequested();
}

void Playlist::deleteItem(const unsigned int index)
{
    if(m_currentMediaIndex == index)
        playPreviousMedia();
    delete m_items[index];
    m_items.remove(index);
    if(m_items.empty())
        emit SignalManager::instance().playlistEjectPlayer();
    for(size_t IPlaylistItem = index; IPlaylistItem < m_items.size(); ++IPlaylistItem)
    {
        m_items[IPlaylistItem]->setIndex(IPlaylistItem);
    }
}

void Playlist::playMedia(const QString& filePath)
{
    emit openMediaFileRequested(filePath);
    for(size_t IMedia = 0; IMedia < m_items.size(); ++IMedia){
        if(IMedia != m_currentMediaIndex)
            m_items[IMedia]->setCurrentMedia(false);
        else
            m_items[IMedia]->setCurrentMedia(true);
    }
}

void Playlist::playPreviousMedia()
{
    if(m_currentMediaIndex > 0)
    {
        m_currentMediaIndex--;
        m_items[m_currentMediaIndex]->playMedia();
    }
}

void Playlist::playNextMedia()
{
    if(m_currentMediaIndex < m_items.size()-1)
    {
        m_currentMediaIndex++;
        m_items[m_currentMediaIndex]->playMedia();
    }
}

void Playlist::updateItemIndices()
{
    for (int i = 0; i < m_items.size(); ++i) {
        m_items[i]->setIndex(i);
    }
}

void Playlist::updateLayout()
{
    while (QLayoutItem *item = m_itemsLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr);
        }
        delete item;
    }

    for (auto *item : std::as_const(m_items)) {
        m_itemsLayout->addWidget(item);
        
    }

    this->updateGeometry();
}
