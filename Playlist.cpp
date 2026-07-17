#include "Playlist.h"
#include "PrefManager.h"
#include "./ToolbarButtons/ToolbarButton.h"
#include "FileFormatManager.h"
#include "Project/ProjectManager.h"
#include "GenericDialog.h"

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
    QStringList collected;

    QDir dir(path);
    if (!dir.exists())
        return collected;

    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QString ext = QFileInfo(filePath).suffix().toLower();
        if (FileFormatManager::instance().isFormatAccepted(ext)) {
            collected.append(filePath);
        }
    }

    return collected;
}
}

Playlist::Playlist(QWidget *parent)
    : QWidget{parent}
{
    m_isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

#ifdef Q_OS_MAC
    QColor colorBtn = qApp->palette().color(QPalette::Button);
    QColor enhancedColor = m_isDarkMode ? colorBtn.lighter(150) : colorBtn.darker(150);
    m_palbtnColor = m_isDarkMode ? enhancedColor : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? QString(enhancedColor.name()) : "palette(mid)";
#else
    m_palbtnColor = m_isDarkMode ? "palette(button)" : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? "palette(button)" : "palette(mid)";

#endif

    setAcceptDrops(true);
    m_mainLayout = new QVBoxLayout();
    this->setLayout(m_mainLayout);

    QHBoxLayout *playlistLabelLayout = new QHBoxLayout();
    m_mainLayout->addLayout(playlistLabelLayout);

    QLabel *playlistLabel = new QLabel();
    QFont playlistFont = playlistLabel->font();
    playlistFont.setPointSize(12);
    playlistFont.setBold(true);
    playlistLabel->setFont(playlistFont);
    //playlistLabel->setTextFormat(Qt::RichText);
    playlistLabel->setText("<b>"+PrefManager::instance().getText("playlist")+"</b>");
    playlistLabelLayout->addWidget(playlistLabel);

    m_loopItemBtn = new ToolbarToggleButton(this,
        false,
        "playlist_loop_white",
        PrefManager::instance().getText("tooltip_loop_playlist") + " " + PrefManager::instance().getText("(activated)"),
        "playlist_loop_white",
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

    // [Bouton] Supprimer tous les éléments
    m_deleteAllBtn = new QPushButton;
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        m_deleteAllBtn->setIcon(QIcon(":/icons/delete_white"));
    } else {
        m_deleteAllBtn->setIcon(QIcon(":/icons/delete"));
    }
    m_deleteAllBtn->setFixedSize(24,24);
    m_deleteAllBtn->setMaximumWidth(24);
    m_deleteAllBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover{"
        "   background-color: tomato;"
        "   border: 1px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}");
    m_deleteAllBtn->setToolTip(PrefManager::instance().getText("tooltip_remove_all_items_playlist"));
    playlistLabelLayout->addWidget(m_deleteAllBtn);

    // [Bouton] Ajouter un élément à la playlist
    m_addItemBtn = new QPushButton;
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        m_addItemBtn->setIcon(QIcon(":/icons/plus_white"));
    } else {
        m_addItemBtn->setIcon(QIcon(":/icons/plus"));
    }
    m_addItemBtn->setToolTip(PrefManager::instance().getText("tooltip_add_item_playlist"));
    m_addItemBtn->setFixedHeight(50);
    m_addItemBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: 2px dashed " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover{"
        "   background-color: " + m_palbtnColorStr + ";"
        "   border: 2px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}");
    playlistLabelLayout->addWidget(m_addItemBtn);

    // LAYOUT ITEMS
    m_itemsLayout = new QVBoxLayout();
    m_mainLayout->addLayout(m_itemsLayout);
    m_mainLayout->addWidget(m_addItemBtn);

    QHBoxLayout *playlistLabelBottomLayout = new QHBoxLayout();
    playlistLabelBottomLayout->addStretch();
    m_playlistTotalDurationLabel = new QLabel();
    m_playlistTotalDurationLabel->setText(PrefManager::instance().getText("duration") + " : 00:00:00");
    playlistLabelBottomLayout->addWidget(m_playlistTotalDurationLabel);

    m_mainLayout->addLayout(playlistLabelBottomLayout);

    m_mainLayout->addStretch();

    connect(m_addItemBtn, &ToolbarButton::clicked, this, &Playlist::addItemDialog);
    connect(m_deleteAllBtn, &ToolbarButton::clicked, this, &Playlist::deleteAllItemsDialog);
    connect(&SignalManager::instance(), &SignalManager::mediaWidgetMediaFinished, this, &Playlist::playNextMedia);
    connect(&SignalManager::instance(), &SignalManager::addPlaylistItems, this, &Playlist::addItemsViaButton);
    connect(&SignalManager::instance(), &SignalManager::requestPlaylistSize, this, [this](){
        if(m_items.size() > 1) emit SignalManager::instance().playlistSizeResponse();
    });
    connect(this, &Playlist::playlistItemCountChanged, this, &Playlist::updateDurationPlaylist);

}

void Playlist::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    qDebug() << "Playlist height : " << this->height();
}

QStringList Playlist::dataHasValidUrls(const QMimeData *mimeData) const{
    QList<QUrl> urlList = mimeData->urls();
    QStringList filePaths;

    for (const QUrl &url : urlList) {
        QString filePath = url.toLocalFile();

        QFileInfo info(filePath);
        if (info.isDir()) {
            filePaths.append(collectValidFilesFromPath(filePath));
        } else if (info.isFile() && FileFormatManager::instance().isFormatAccepted(info.suffix())) {
            filePaths.append(filePath);
        }
    }
    return filePaths;
}

void Playlist::dragEnterEvent(QDragEnterEvent *event){
    if (!event->mimeData()->hasFormat("move-PlaylistItem") && event->mimeData()->hasUrls()) {
        QStringList filePaths = dataHasValidUrls(event->mimeData());
        if(filePaths.empty())
            event->ignore();
    }

    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("move-PlaylistItem")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

int Playlist::visualDroppedIndex(const QPoint &dropPos) const
{
    // On itère sur les éléments de la playlist pour déterminer l'index visuel où l'élément a été déposé
    for (int i = 0; i < m_itemsSortOrder.size(); ++i) {
        QWidget *playlistItemWidget = m_items[m_itemsSortOrder[i]];
        QRect itemRect = playlistItemWidget->geometry();
        int midY = itemRect.top() + itemRect.height() / 2;

        // Si la position de dépôt est au-dessus du milieu de cet élément, on retourne l'index visuel actuel
        if (dropPos.toPointF().y() < midY) {
            return i;
        }
    }

    return m_itemsSortOrder.isEmpty() ? 0 : m_itemsSortOrder.size();
}

void Playlist::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    qDebug() << "Old sort order: " << m_itemsSortOrder;

    // Cas où un élément de la playlist est déplacé
    if (mimeData->hasFormat("move-PlaylistItem")) {
        // Récupération de l'index de l'élément déplacé à partir des données MIME
        int oldItemIndex = mimeData->data("move-PlaylistItem").toInt();
        qDebug() << "Dragged item index: " << oldItemIndex;

        int newItemIndex = -1; // Initialisation de l'index de l'élément déplacé à -1 (invalide)
        const int visualDropIndex = visualDroppedIndex(event->pos()); // Récupération de l'index visuel où l'élément a été déposé
        qDebug() << "Visual drop index: " << visualDropIndex;

        // Convert visualDropIndex to the new item position to use move()
        newItemIndex = visualDropIndex == 0 ? 0 : visualDropIndex-1;

        // Move the dragged item to the new position in the sort order
        if (newItemIndex != -1) {
            const int targetVisibleIndex = m_itemsSortOrder.indexOf(newItemIndex);
            if (targetVisibleIndex != oldItemIndex && targetVisibleIndex >= 0) {

                m_itemsSortOrder.move(oldItemIndex, newItemIndex);
                m_itemsShuffleOrder.move(oldItemIndex, newItemIndex);

                if(m_currentMediaIndex > oldItemIndex && m_currentMediaIndex <= newItemIndex) {
                    m_currentMediaIndex--;
                    qDebug() << "Current media - : " << m_currentMediaIndex;
                }
                else if(m_currentMediaIndex < oldItemIndex && m_currentMediaIndex >= newItemIndex) {
                    m_currentMediaIndex++;
                    qDebug() << "Current media + : " << m_currentMediaIndex;
                }
                else if(m_currentMediaIndex == oldItemIndex) {
                    m_currentMediaIndex = newItemIndex;
                }

                qDebug() << "Updated sort order: " << m_itemsSortOrder;
            }
        }

        updateItemIndices();
        updateLayout();

        event->acceptProposedAction();
    } else if (mimeData->hasUrls()) {
        QStringList filePaths = dataHasValidUrls(event->mimeData());
        if(filePaths.size() < event->mimeData()->urls().size()){
            QMessageBox::warning(this, "", PrefManager::instance().getText("messagebox_format_not_accepted"));
        }
        if (!filePaths.isEmpty()) {
            const int insertionIndex = visualDroppedIndex(event->pos());

            const bool wasEmpty = m_items.empty();

            insertItemsFromPaths(filePaths, insertionIndex);

            if (wasEmpty && !filePaths.empty()) {
                m_items[m_itemsSortOrder[0]]->playMedia();
            }

            updateItemIndices();
            updateLayout();
            emit disableToolbarLoopRequested();
            if (m_items.size() > 1) {
                emit SignalManager::instance().activateMediaChangeBtn(true);
            }
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
        FileFormatManager::instance().getOpenFileDialogFilters()
    );

    if(filesPaths.empty()){
        qDebug() << "PLAYLIST - Pas de fichier sélectionné";
        return;
    }

    QFileInfo fileInfo (filesPaths[0]);
    prefManager.setPref("Paths", "lp_open_media", fileInfo.absolutePath());


    addItemsViaButton(filesPaths);

}

void Playlist::insertItemsFromPaths(const QStringList &filesPaths, int insertionIndex)
{
    const int targetIndex = insertionIndex < 0 ? m_items.size() : insertionIndex;

    for (int i = 0; i < filesPaths.size(); ++i) {
        PlaylistItem *newItem = new PlaylistItem(nullptr, filesPaths.at(i));
        newItem->setIndex(m_items.size());
        m_items.append(newItem);

        const int orderIndex = targetIndex + i;
        const unsigned int itemIndex = static_cast<unsigned int>(m_items.size() - 1);
        if (orderIndex >= 0 && orderIndex < m_itemsSortOrder.size()) {
            m_itemsSortOrder.insert(orderIndex, itemIndex);
            m_itemsShuffleOrder.insert(orderIndex, itemIndex);
        } else {
            m_itemsSortOrder.append(itemIndex);
            m_itemsShuffleOrder.append(itemIndex);
        }

        connect(newItem, &PlaylistItem::deleteItemRequested, this, &Playlist::deleteItem);
        connect(newItem, &PlaylistItem::updatePlaylistCurrentIndex, this, [this](unsigned int index){
            m_currentMediaIndex = index;
        });
        connect(newItem, &PlaylistItem::playPlaylistItemRequested, this, &Playlist::playMedia);
        connect(newItem, &PlaylistItem::durationParsed, this, &Playlist::updateDurationPlaylist);
    }

    emit playlistItemCountChanged();
}

void Playlist::addItemsViaButton(const QStringList &filesPaths)
{
    qDebug() << "PLAYLIST - Fichiers sélectionnés : " << filesPaths;
    const bool wasEmpty = m_items.empty();

    insertItemsFromPaths(filesPaths, m_items.size());

    if(wasEmpty && !filesPaths.empty()){
        m_items[m_itemsSortOrder[0]]->playMedia();
    }
    if(!filesPaths.empty() && m_sortButtons->checkedButton())
        sortPlaylist(m_sortButtons->checkedId(), true);
    if(m_playlistShuffled){
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(m_itemsShuffleOrder.begin(), m_itemsShuffleOrder.end(), g);
        
        // Make current media the first item in the shuffle order
        if(!m_itemsShuffleOrder.empty()){
            unsigned int currentItemId = m_itemsSortOrder[m_currentMediaIndex];
            int currentShufflePos = m_itemsShuffleOrder.indexOf(currentItemId);
            if(currentShufflePos >= 0)
                m_itemsShuffleOrder.swapItemsAt(0, currentShufflePos);
        }
    }

    updateItemIndices();
    updateLayout();

    emit disableToolbarLoopRequested();
    if(m_items.size() > 1)
        emit SignalManager::instance().activateMediaChangeBtn(true);

    // qDebug() << "PLAYLIST - Liste m_items : " << m_items;
    // qDebug() << "PLAYLIST - Liste m_itemsSortOrder : " << m_itemsSortOrder;
    // qDebug() << "PLAYLIST - Liste m_itemsShuffleOrder : " << m_itemsShuffleOrder;
}

void Playlist::deleteAllItemsDialog()
{

    auto& prefManager = PrefManager::instance();

    QDialog dialog;
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setAttribute(Qt::WA_TranslucentBackground);
    dialog.setFixedSize(500,220);
    dialog.setObjectName("dialogWindow");
    dialog.setStyleSheet(
        "#dialogWindow {"
            "background-color: transparent;"
        "}"
    );

    QVBoxLayout outerLayout(&dialog);
    outerLayout.setContentsMargins(0,0,0,0);

    QWidget* container = new QWidget;
    container->setObjectName("dialogContainer");

    container->setStyleSheet(
        "#dialogContainer {"
            "background-color: palette(Window);"
            "border: 2px solid " + m_palbtnColorStr + ";"
            "border-radius: 20px;"
        "}"
    );

    outerLayout.addWidget(container);

    QVBoxLayout layout(container);
    layout.setContentsMargins(20,20,20,20);

    QLabel* titleLabel = new QLabel;
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setText(prefManager.getText("remove_all_items_playlist"));

    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout.addWidget(titleLabel);

    QLabel* textLabel = new QLabel;
    QFont textFont = textLabel->font();
    textFont.setPointSize(10);
    textLabel->setFont(textFont);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setTextFormat(Qt::RichText);
    textLabel->setText(
                        prefManager.getText("remove_all_items") + "<br>" +
                        "<i>"+ prefManager.getText("delete_all_items_confirm") +"</i><br>");
    layout.addWidget(textLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* deleteAllBtn = new QPushButton(prefManager.getText("remove_all"));
    QPushButton* cancelBtn = new QPushButton(prefManager.getText("generic_dialog_btn_cancel"));

    QFont btnFont = deleteAllBtn->font();
    btnFont.setPointSize(10);
    btnFont.setBold(true);

    deleteAllBtn->setFont(btnFont);
    cancelBtn->setFont(btnFont);

    deleteAllBtn->setFixedSize(200,40);
    cancelBtn->setFixedSize(200,40);

    deleteAllBtn->setStyleSheet("QPushButton{"
        "   background-color: tomato;"
        "   border: 1px solid tomato;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover{"
        "   background-color: salmon;"
        "   border: 2px solid salmon;"
        "   border-radius: 4px;"
        "}"
    );

    cancelBtn->setStyleSheet("QPushButton{"
        "   background-color: palette(Window);"
        "   border: 1px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover{"
        "   background-color: " + m_palbtnColorStr + ";"
        "   border: 2px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}"
    );

    connect(deleteAllBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    btnLayout->addWidget(deleteAllBtn);
    btnLayout->setSpacing(20);
    btnLayout->addWidget(cancelBtn);
    layout.addLayout(btnLayout);

    if (dialog.exec() == QDialog::Accepted) {
        deleteAllItems();
    }
}

void Playlist::deleteAllItems()
{
    // while(!m_items.isEmpty()){
    //     deleteItem(static_cast<unsigned int>(m_items.size() - 1));
    // }

    for(size_t IItem = 0; IItem < m_items.size(); ++IItem){
        m_items[IItem]->deleteLater();
    }

    emit ejectCurrentMedia();
    m_currentMediaIndex = 0;
    m_items.clear();
    m_itemsShuffleOrder.clear();
    m_itemsSortOrder.clear();
    emit SignalManager::instance().activateMediaChangeBtn(false);
    emit playlistItemCountChanged();
}

void Playlist::deleteItem(const unsigned int index)
{
    if(m_items.size() > 1 && m_currentMediaIndex == index){
        playPreviousMedia();
        if(m_currentMediaIndex == 0)
            playNextMedia();
    }
    if(m_currentMediaIndex >= index && m_currentMediaIndex > 0)
        m_currentMediaIndex--;
    m_items[m_itemsSortOrder[index]]->deleteLater();
    m_items.remove(m_itemsSortOrder.indexOf(index));
    m_itemsShuffleOrder.removeAll(index);
    m_itemsSortOrder.removeAll(index);
    if(m_items.empty())
        emit SignalManager::instance().playlistEjectPlayer();
    for(size_t IPlaylistItem = 0; IPlaylistItem < m_items.size(); ++IPlaylistItem)
    {
        if(m_itemsSortOrder[IPlaylistItem] > index) m_itemsSortOrder[IPlaylistItem]--;
        if(m_itemsShuffleOrder[IPlaylistItem] > index) m_itemsShuffleOrder[IPlaylistItem]--;
        m_items[m_itemsSortOrder[IPlaylistItem]]->setIndex(IPlaylistItem);
    }
    if(m_items.size() <= 1)
        emit SignalManager::instance().activateMediaChangeBtn(false);
    emit playlistItemCountChanged();
}

void Playlist::playMedia(const QString& filePath, const bool isClicked)
{
    ProjectManager& projManager = ProjectManager::instance();
    bool changeMedia = true;
    if(projManager.needSave()){
        SLV::showGenericDialog(
            this,
            PrefManager::instance().getText("dialog_save_project_dialog_title"),
            PrefManager::instance().getText("dialog_save_project_dialog_text"),

            [&projManager]() {
                projManager.saveProject(false);
            },

            [](){},
            [&changeMedia](){ changeMedia = false; }
        );
    }
    if(!changeMedia) return;
    emit openMediaFileRequested(filePath);
    for(size_t IMedia = 0; IMedia < m_items.size(); ++IMedia){
        if(IMedia != m_currentMediaIndex)
            m_items[m_itemsSortOrder[IMedia]]->setCurrentMedia(false);
        else
            m_items[m_itemsSortOrder[IMedia]]->setCurrentMedia(true);
    }
}

void Playlist::playPreviousMedia()
{
    ProjectManager& projManager = ProjectManager::instance();
    bool changeMedia = true;
    if(projManager.needSave()){
        SLV::showGenericDialog(
            this,
            PrefManager::instance().getText("dialog_save_project_dialog_title"),
            PrefManager::instance().getText("dialog_save_project_dialog_text"),

            [&projManager]() {
                projManager.saveProject(false);
            },

            [](){},
            [&changeMedia](){ changeMedia = false; }
        );
    }
    if(!changeMedia) return;
    if(m_playlistShuffled){
        unsigned int currentItemId = m_itemsSortOrder[m_currentMediaIndex];
        int currentShufflePos = m_itemsShuffleOrder.indexOf(currentItemId);
        if(currentShufflePos <= 0)
            return;
        m_currentMediaIndex = m_itemsSortOrder.indexOf(m_itemsShuffleOrder[currentShufflePos - 1]);
    }
    else{
        if(m_currentMediaIndex == 0)
            return;
        m_currentMediaIndex--;
    }
    m_items[m_itemsSortOrder[m_currentMediaIndex]]->playMedia();
    
}

void Playlist::playNextMedia()
{
    ProjectManager& projManager = ProjectManager::instance();
    bool changeMedia = true;
    if(projManager.needSave()){
        SLV::showGenericDialog(
            this,
            PrefManager::instance().getText("dialog_save_project_dialog_title"),
            PrefManager::instance().getText("dialog_save_project_dialog_text"),

            [&projManager]() {
                projManager.saveProject(false);
            },

            [](){},
            [&changeMedia](){ changeMedia = false; }
        );
    }
    if(!changeMedia) return;

    bool hasNext = false;
    if(m_playlistShuffled){
        unsigned int currentItemId = m_itemsSortOrder[m_currentMediaIndex];
        int currentShufflePos = m_itemsShuffleOrder.indexOf(currentItemId);
        if(currentShufflePos >= 0 && currentShufflePos + 1 < m_itemsShuffleOrder.size()){
            m_currentMediaIndex = m_itemsSortOrder.indexOf(m_itemsShuffleOrder[currentShufflePos + 1]);
            hasNext = true;
        }
        else return;
    } else {
        if(m_currentMediaIndex < m_items.size()-1){
            m_currentMediaIndex++;
            hasNext = true;
        }
    }

    if(hasNext){
        m_items[m_itemsSortOrder[m_currentMediaIndex]]->playMedia();
        return;
    }
    
    if(m_playlistLooping){
        ProjectManager& projManager = ProjectManager::instance();
        bool changeMedia = true;
        if(projManager.needSave()){
            SLV::showGenericDialog(
                this,
                PrefManager::instance().getText("dialog_save_project_dialog_title"),
                PrefManager::instance().getText("dialog_save_project_dialog_text"),

                [&projManager]() {
                    projManager.saveProject(false);
                },

                [](){},
                [&changeMedia](){ changeMedia = false; }
            );
        }
        if(!changeMedia) return;

        if(m_playlistShuffled){
            unsigned int firstItemId = m_itemsShuffleOrder.value(0, 0u);
            m_currentMediaIndex = m_itemsSortOrder.indexOf(firstItemId); // If the shuffle order is empty, current media index is set to 0
        } else {
            m_currentMediaIndex = 0;
        }
        m_items[m_itemsSortOrder[m_currentMediaIndex]]->playMedia();

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
    if(!m_itemsShuffleOrder.isEmpty()){
        // Make current media the first item in the shuffle list
        unsigned int currentItemId = m_itemsSortOrder[m_currentMediaIndex];
        int currentShufflePos = m_itemsShuffleOrder.indexOf(currentItemId);
        if(currentShufflePos >= 0)
            m_itemsShuffleOrder.move(currentShufflePos, 0);
    }
}

void Playlist::disableShuffle(){
    m_shuffleItemBtn->setButtonState(false);
    m_playlistShuffled = false;
    m_itemsShuffleOrder = m_itemsSortOrder;
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
    int currentMedia = m_itemsSortOrder[m_currentMediaIndex];
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
            m_currentMediaIndex = m_itemsSortOrder.indexOf(currentMedia);
        for(size_t IItem = 0 ; IItem < m_items.size(); ++IItem){
            m_itemsShuffleOrder[IItem] = m_itemsShuffleOrder[m_itemsSortOrder[IItem]];
        }
        updateItemIndices();
        updateLayout();
    }
}

void Playlist::updateDurationPlaylist()
{
    qint64 totalDuration = 0;
    for (const auto& item : m_items) {
        totalDuration += item->getDuration();
    }

    QString time = TimeFormatter::msToHHMMSSFF(totalDuration, 1);
    QString timeChopped = time.left(qMax(0, time.length() - 3));

    m_playlistTotalDurationLabel->setText(PrefManager::instance().getText("duration") + " : " + timeChopped);
}