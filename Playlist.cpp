#include "Playlist.h"
#include "PrefManager.h"
#include "./ToolbarButtons/ToolbarButton.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDir>
#include <QDirIterator>
#include <QCheckBox>

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
    playlistLabelLayout->addWidget(playlistLabel);

    // [Bouton] Boucle -> Lorsqu'on atteint la fin de la playlist, celle-ci se rejoue du début

    // TODO : Implémenter la fonction 

    m_loopItemBtn = new ToolbarToggleButton(this, 
        false, 
        "loop_off_white", 
        PrefManager::instance().getText("tooltip_loop_playlist") + " " + PrefManager::instance().getText("(activated)"),
        "loop_off_white", 
        PrefManager::instance().getText("tooltip_loop_playlist") + " " + PrefManager::instance().getText("(deactivated)"));
    m_loopItemBtn->setFixedSize(24,24);
    m_loopItemBtn->setToggledIconFrame(true);
    connect(m_loopItemBtn, &ToolbarToggleButton::stateActivated, this, &Playlist::enableLoop);
    connect(m_loopItemBtn, &ToolbarToggleButton::stateDeactivated, this, &Playlist::disableLoop);
    playlistLabelLayout->addWidget(m_loopItemBtn);

    // [Bouton] Lecture aléatoire -> Lecture aléatoire de la playlist (1 fois chaque élément)

    // TODO : Implémenter la fonction 

    m_shuffleItemBtn = new ToolbarToggleButton(this, 
        false, 
        "shuffle_white", 
        PrefManager::instance().getText("tooltip_shuffle_playlist") + " " + PrefManager::instance().getText("(activated)"),
        "shuffle_white", 
        PrefManager::instance().getText("tooltip_shuffle_playlist") + " " + PrefManager::instance().getText("(deactivated)"));
    m_shuffleItemBtn->setFixedSize(24,24);
    m_shuffleItemBtn->setToggledIconFrame(true);
    connect(m_shuffleItemBtn, &ToolbarToggleButton::stateActivated, this, &Playlist::enableShuffle);
    connect(m_shuffleItemBtn, &ToolbarToggleButton::stateDeactivated, this, &Playlist::disableShuffle);
    playlistLabelLayout->addWidget(m_shuffleItemBtn);

    // [Bouton] Trier la playlist -> Ordre A-Z, Z-A, récent
    QVBoxLayout* sortPlaylistLayout = new QVBoxLayout();

    // TODO : 
    // 1/ Quand une des checkboxes est cochée,
    // Les autres doivent être désactivées

    // 2/ Quand un élément est ajouté à la playlist OU qu'un élément est déplacé,
    // Il faut que les checkboxes soient décochées

    // 3/ Il faudrait que le popup soit en bas du bouton, pas en haut (peut-être auto en bas si on met plus de checkboxes)
    
    // 4/ Possible de garder en mémoire, l'ordre d'ajout ?

    QCheckBox* sortAZCheckBox = new QCheckBox(PrefManager::instance().getText("sort_AZ_playlist"));
    QCheckBox* sortZACheckBox = new QCheckBox(PrefManager::instance().getText("sort_ZA_playlist"));
    sortPlaylistLayout->addWidget(sortAZCheckBox);
    sortPlaylistLayout->addWidget(sortZACheckBox);
    
    m_sortPlaylistBtn = new ToolbarPopupButton(this,
        sortPlaylistLayout, 
        "sort_white", 
        PrefManager::instance().getText("tooltip_sort_playlist"));
    m_sortPlaylistBtn->setFixedSize(24,24);
    playlistLabelLayout->addWidget(m_sortPlaylistBtn);    

    // [Bouton] Ajouter un élément à la playlist
    m_addItemBtn = new QPushButton;
    m_addItemBtn->setIcon(QIcon(":/icons/plus_white"));
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
            // updateItemIndices();
            // updateLayout();
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Playlist::addItemDialog()
{

    auto& prefManager = PrefManager::instance();
    QStringList filesPaths = QFileDialog::getOpenFileNames(
        this, 
        prefManager.getText("dialog_open_files"), 
        prefManager.getPref("Paths", "lp_open_media"), 
        "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.wav)"
    ); 

    if(filesPaths.empty()){
        qDebug() << "PLAYLIST - Pas de fichier sélectionné";
        return;
    }

    QFileInfo fileInfo (filesPaths[0]);
    prefManager.setPref("Paths", "lp_open_media", fileInfo.absolutePath());


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

void Playlist::enableLoop(){
    m_loopItemBtn->setButtonState(true);
}

void Playlist::disableLoop(){
    m_loopItemBtn->setButtonState(false);
}

void Playlist::enableShuffle(){
    m_shuffleItemBtn->setButtonState(true);
}

void Playlist::disableShuffle(){
    m_shuffleItemBtn->setButtonState(false);
}
