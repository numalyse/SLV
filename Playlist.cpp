#include "Playlist.h"

Playlist::Playlist(QWidget *parent)
    : QWidget{parent}
{
    m_mainLayout = new QVBoxLayout(this);
    QHBoxLayout *playlistLabelLayout = new QHBoxLayout();
    QLabel *playlistLabel = new QLabel();
    playlistLabel->setTextFormat(Qt::RichText);
    playlistLabel->setText("<b>Playlist</b>");
    playlistLabelLayout->addWidget(playlistLabel); //ajouter les boutons random et loop peut-être
    m_mainLayout->addLayout(playlistLabelLayout);
    m_addItemBtn = new QPushButton("+");
    m_itemsLayout = new QVBoxLayout();
    m_mainLayout->addLayout(m_itemsLayout);
    m_mainLayout->addWidget(m_addItemBtn);
    m_mainLayout->addStretch();

    connect(m_addItemBtn, &QPushButton::clicked, this, &Playlist::addItemDialog);
}

void Playlist::addItemDialog()
{
    QStringList filesPaths = QFileDialog::getOpenFileNames(this, "Ouvrir des fichiers multimédia", "/", "Fichiers multimédia (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)");
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

        connect(newItem, &PlaylistItem::deleteItemRequested, this, &Playlist::deleteItem);
        connect(newItem, &PlaylistItem::updatePlaylistCurrentIndex, this, [&](unsigned int index){
            m_currentMediaIndex = index;
        });
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
