#include "Playlist.h"
#include "PrefManager.h"
#include "./ToolbarButtons/ToolbarButton.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDir>
#include <QDirIterator>
#include <QCheckBox>
#include <QStyleHints>
#include <QGuiApplication>

namespace {
QStringList collectValidFilesFromPath(const QString &path)
{
    const QStringList allowedExtensions = {"mp4", "avi", "mkv", "mov", "m4v", "vob", "png", "jpg", "wav", "mp3"};
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

    createSortBtn();
    playlistLabelLayout->addWidget(m_sortPlaylistBtn);

    // [Bouton] Ajouter un élément à la playlist
    m_addItemBtn = new QPushButton;
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        m_addItemBtn->setIcon(QIcon(":/icons/plus_white"));
    } else {
        m_addItemBtn->setIcon(QIcon(":/icons/plus"));
    }
    m_addItemBtn->setToolTip(PrefManager::instance().getText("tooltip_add_item_playlist"));
    m_addItemBtn->setFixedHeight(50);
    QString color = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "palette(button);" : "black;";
    m_addItemBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: 2px dashed " + color +
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover{"
        "   background-color: palette(button);"
        "   border: 2px solid " + color +
        "   border-radius: 4px;"
        "}");
    playlistLabelLayout->addWidget(m_addItemBtn);

    m_itemsLayout = new QVBoxLayout();
    m_mainLayout->addLayout(m_itemsLayout);
    m_mainLayout->addWidget(m_addItemBtn);
    m_mainLayout->addStretch();

    connect(m_addItemBtn, &ToolbarButton::clicked, this, &Playlist::addItemDialog);
    connect(&SignalManager::instance(), &SignalManager::mediaWidgetMediaFinished, this, &Playlist::playNextMedia);
    connect(&SignalManager::instance(), &SignalManager::addPlaylistItems, this, &Playlist::addItemsFromPaths);

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
            // PlaylistItem *draggedItem = m_items.takeAt(draggedIndex);
            // m_items.insert(dropIndex, draggedItem);
            if(m_sortButtons->checkedButton()){
                m_sortButtons->setExclusive(false);
                m_sortButtons->checkedButton()->setChecked(false);
            }
            if(m_currentMediaIndex == draggedIndex) m_currentMediaIndex = m_itemsSortOrder.indexOf(dropIndex);
            m_itemsSortOrder.swapItemsAt(draggedIndex, m_itemsSortOrder.indexOf(dropIndex));
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
        "Fichiers vidéo (*.mp4 *.avi *.mkv *.mov *.m4v *.vob *.png *.jpg *.wav *.mp3)"
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
    const bool wasEmpty = m_items.empty();
    for(size_t IFilePath = 0; IFilePath < filesPaths.size(); ++IFilePath){
        PlaylistItem *newItem = new PlaylistItem(nullptr, filesPaths.at(IFilePath));
        newItem->setIndex(m_items.size());
        m_items.append(newItem);
        m_itemsLayout->addWidget(newItem);
        m_itemsShuffleOrder.append(m_items.size()-1);
        m_itemsSortOrder.append(m_items.size()-1);
        this->adjustSize();
        this->updateGeometry();
        
        connect(newItem, &PlaylistItem::deleteItemRequested, this, &Playlist::deleteItem);
        connect(newItem, &PlaylistItem::updatePlaylistCurrentIndex, this, [this](unsigned int index){
            m_currentMediaIndex = index;
        });
        connect(newItem, &PlaylistItem::playPlaylistItemRequested, this, &Playlist::playMedia);
    }
    if(wasEmpty && !filesPaths.empty()){
        m_items[m_itemsSortOrder[0]]->playMedia();
    }
    if(!filesPaths.empty() && m_sortButtons->checkedButton())
        sortPlaylist(m_sortButtons->checkedId(), true);
    if(m_playlistShuffled){
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(m_itemsShuffleOrder.begin(), m_itemsShuffleOrder.end(), g);
        if(!m_itemsShuffleOrder.empty())
            m_itemsShuffleOrder.swapItemsAt(0, m_currentMediaIndex);
        m_currentMediaIndex = 0;
    }
    emit disableToolbarLoopRequested();
}

void Playlist::deleteItem(const unsigned int index)
{
    if(m_currentMediaIndex == index){
        playPreviousMedia();
        if(m_currentMediaIndex == 0)
                playNextMedia();
    }
    if(m_currentMediaIndex >= index)
        m_currentMediaIndex--;
    m_items[m_itemsSortOrder[index]]->deleteLater();
    m_items.remove(m_itemsSortOrder.indexOf(index));
    m_itemsShuffleOrder.removeAll(index);
    m_itemsSortOrder.removeAll(index);
    if(m_items.empty())
        emit SignalManager::instance().playlistEjectPlayer();
    for(size_t IPlaylistItem = index; IPlaylistItem < m_items.size(); ++IPlaylistItem)
    {
        if(m_itemsSortOrder[IPlaylistItem] > index) m_itemsSortOrder[IPlaylistItem]--;
        if(m_itemsShuffleOrder[IPlaylistItem] > index) m_itemsShuffleOrder[IPlaylistItem]--;
        m_items[m_itemsSortOrder[IPlaylistItem]]->setIndex(IPlaylistItem);
    }
}

void Playlist::playMedia(const QString& filePath, const bool isClicked)
{
    emit openMediaFileRequested(filePath);
    for(size_t IMedia = 0; IMedia < m_items.size(); ++IMedia){
        unsigned int index = m_playlistShuffled && !isClicked ? m_itemsShuffleOrder[m_currentMediaIndex] : m_currentMediaIndex;
        if(IMedia != index)
            m_items[m_itemsSortOrder[IMedia]]->setCurrentMedia(false);
        else
            m_items[m_itemsSortOrder[IMedia]]->setCurrentMedia(true);
    }
}

void Playlist::playPreviousMedia()
{
    if(m_currentMediaIndex > 0)
    {
        m_currentMediaIndex--;
        unsigned int index = m_playlistShuffled ? m_itemsShuffleOrder[m_currentMediaIndex] : m_currentMediaIndex;
        m_items[m_itemsSortOrder[index]]->playMedia();
    }
}

void Playlist::playNextMedia()
{
    if(m_currentMediaIndex < m_items.size()-1)
    {
        m_currentMediaIndex++;
        unsigned int index = m_playlistShuffled ? m_itemsShuffleOrder[m_currentMediaIndex] : m_currentMediaIndex;
        m_items[m_itemsSortOrder[index]]->playMedia();
    }
    else if(m_playlistLooping){
        m_currentMediaIndex = 0;
        unsigned int index = m_playlistShuffled ? m_itemsShuffleOrder[m_currentMediaIndex] : m_currentMediaIndex;
        m_items[m_itemsSortOrder[index]]->playMedia();
    }
}

void Playlist::updateItemIndices()
{
    for (int i = 0; i < m_items.size(); ++i) {
        m_items[m_itemsSortOrder[i]]->setIndex(i);
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

    // for (auto *item : std::as_const(m_items)) {
    //     m_itemsLayout->addWidget(item);
        
    // }

    for(size_t IItem = 0; IItem < m_items.size(); ++IItem){
        m_itemsLayout->addWidget(m_items[m_itemsSortOrder[IItem]]);
    }

    this->updateGeometry();
}

void Playlist::enableLoop(){
    m_loopItemBtn->setButtonState(true);
    m_playlistLooping = true;
}

void Playlist::disableLoop(){
    m_loopItemBtn->setButtonState(false);
    m_playlistLooping = false;
}

void Playlist::enableShuffle(){
    m_shuffleItemBtn->setButtonState(true);
    m_playlistShuffled = true;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_itemsShuffleOrder.begin(), m_itemsShuffleOrder.end(), g);
    if(!m_itemsShuffleOrder.empty())
        m_itemsShuffleOrder.swapItemsAt(0, m_currentMediaIndex);
    m_currentMediaIndex = 0;
}

void Playlist::disableShuffle(){
    m_shuffleItemBtn->setButtonState(false);
    m_playlistShuffled = false;
    for(unsigned int IOrder = 0; IOrder < m_items.size(); IOrder++){
        m_itemsShuffleOrder[IOrder] = IOrder;
    }
}

void Playlist::createSortBtn()
{
    // [Bouton] Trier la playlist -> Ordre A-Z, Z-A, récent
    QVBoxLayout* sortPlaylistLayout = new QVBoxLayout();

    m_sortButtons = new QButtonGroup();

    QRadioButton* sortAdditionDownRadioBtn = new QRadioButton(PrefManager::instance().getText("sort_addition_down_playlist"));
    QRadioButton* sortAdditionUpRadioBtn = new QRadioButton(PrefManager::instance().getText("sort_addition_up_playlist"));
    QRadioButton* sortAZRadioBtn = new QRadioButton(PrefManager::instance().getText("sort_AZ_playlist"));
    QRadioButton* sortZARadioBtn = new QRadioButton(PrefManager::instance().getText("sort_ZA_playlist"));

    sortAdditionDownRadioBtn->setChecked(true);

    m_sortButtons->addButton(sortAdditionDownRadioBtn, 0);
    m_sortButtons->addButton(sortAdditionUpRadioBtn, 1);
    m_sortButtons->addButton(sortAZRadioBtn, 2);
    m_sortButtons->addButton(sortZARadioBtn, 3);
    sortPlaylistLayout->addWidget(sortAdditionDownRadioBtn);
    sortPlaylistLayout->addWidget(sortAdditionUpRadioBtn);
    sortPlaylistLayout->addWidget(sortAZRadioBtn);
    sortPlaylistLayout->addWidget(sortZARadioBtn);

    connect(m_sortButtons, &QButtonGroup::idToggled, this, &Playlist::sortPlaylist);

    m_sortPlaylistBtn = new ToolbarPopupButton(this,
                                               sortPlaylistLayout,
                                               "sort_white",
                                               PrefManager::instance().getText("tooltip_sort_playlist"));
    m_sortButtons->setParent(m_sortPlaylistBtn);
    m_sortPlaylistBtn->setFixedSize(24,24);
    m_sortPlaylistBtn->setOnTop(false);
}

void Playlist::sortPlaylist(int id, bool checked)
{
    if(checked){
        m_sortButtons->setExclusive(true);
        switch(id){
        case 0:
            std::sort(m_itemsSortOrder.begin(), m_itemsSortOrder.end());
            break;
        case 1:
            std::sort(m_itemsSortOrder.begin(), m_itemsSortOrder.end(), std::greater<unsigned int>());
            break;
        case 2:
            std::sort(m_itemsSortOrder.begin(), m_itemsSortOrder.end(), [this](unsigned int a, unsigned int b){
                return m_items[a]->getTitle() < m_items[b]->getTitle();
            });
            break;
        case 3:
            std::sort(m_itemsSortOrder.begin(), m_itemsSortOrder.end(), [this](unsigned int a, unsigned int b){
                return m_items[a]->getTitle() > m_items[b]->getTitle();
            });
            break;
        }

        if(m_currentMediaIndex < m_items.size())
            m_currentMediaIndex = m_itemsSortOrder[m_currentMediaIndex];
        for(size_t IItem = 0 ; IItem < m_items.size(); ++IItem){
            m_itemsShuffleOrder[IItem] = m_itemsShuffleOrder[m_itemsSortOrder[IItem]];
        }
        updateItemIndices();
        updateLayout();
    }
}
