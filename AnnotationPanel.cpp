#include "AnnotationPanel.h"
#include "AnnotationDialog.h"
#include "IconHelper.h"
#include "Project/ProjectManager.h"

#include <algorithm>

#include <QScrollArea>

AnnotationPanel::AnnotationPanel(ThumbnailWorker* thumbnailWorker, QWidget *parent) : QWidget(parent), p_thumbnailWorker{thumbnailWorker}
{
    auto* annotations = ProjectManager::instance().annotationManager();

    connect(p_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &AnnotationPanel::onThumbnailReady);

    connect(annotations, &AnnotationManager::annotationAdded,   this, &AnnotationPanel::onAnnotationAdded);
    connect(annotations, &AnnotationManager::annotationUpdated, this, &AnnotationPanel::onAnnotationUpdated);
    connect(annotations, &AnnotationManager::annotationRemoved, this, &AnnotationPanel::onAnnotationRemoved);
    connect(annotations, &AnnotationManager::annotationsReset, this, &AnnotationPanel::rebuild);

    // once project fps are parsed, rebuild to update annotations fps
    connect(&ProjectManager::instance(), &ProjectManager::projectInitialized, this, &AnnotationPanel::rebuild);

    connect(this, &AnnotationPanel::addAnnotationRequested, annotations, &AnnotationManager::addAnnotation);
    connect(this, &AnnotationPanel::updateAnnotationRequested, annotations, &AnnotationManager::updateAnnotation);
    connect(this, &AnnotationPanel::removeAnnotationRequested, annotations, &AnnotationManager::removeAnnotation);

    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(4);
    m_layout->setContentsMargins(0, 4, 0, 4);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_addAnnotationBtn = new ToolbarButton(this, "plus_white");
    connect(m_addAnnotationBtn, &QPushButton::clicked, this, &AnnotationPanel::annotationCreationDialog);

    // color buttons shown in the popup when hovering the filter button
    QHBoxLayout* filterColorsLayout = new QHBoxLayout();
    for (const auto& [colorName, color] : IconHelper::colorPalette()) {
        ToolbarToggleButton* colorBtn = new ToolbarToggleButton(this, false, " ", colorName, " ", colorName);
        colorBtn->setIcon(IconHelper::genIconPreviewColor(color, color == QColor{255, 255, 255, 255}));
        connect(colorBtn, &ToolbarButton::clicked, this, [this, colorBtn, color](){ 
            if(colorBtn->isChecked()){
                colorBtn->setStyleSheet(QString("border: 2px solid %1; border-radius: 4px;").arg(color.name()));
                m_filterColors.append(color);
            } 
            else {
                colorBtn->setStyleSheet("");
                m_filterColors.removeAll(color);
            }

            filterBy(m_filterColors); 
        });
        filterColorsLayout->addWidget(colorBtn);
    }

    m_filterByColorBtn = new ToolbarToggleHoverButton(
        this,
        filterColorsLayout,
        false,
        "filter_on_white",
        PrefManager::instance().getText("tooltip_filter_on"),
        "filter_off_white",
        PrefManager::instance().getText("tooltip_filter_off")
    );
    m_filterByColorBtn->setOnTop(false);

    // clicking the button restores the last color filter or disables it
    connect(m_filterByColorBtn, &ToolbarToggleButton::stateActivated, this, [this](){
        if( !m_filterColors.isEmpty() )
            filterBy(m_filterColors);
        else
            m_filterByColorBtn->setButtonState(false); // no color picked yet, nothing to filter
    });
    connect(m_filterByColorBtn, &ToolbarToggleButton::stateDeactivated, this, [this](){
        filterBy({});
    });

    buttonLayout->addWidget(m_addAnnotationBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_filterByColorBtn);

    m_layout->addLayout(buttonLayout);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* itemsContainer = new QWidget(scrollArea);
    m_itemsLayout = new QVBoxLayout(itemsContainer);
    m_itemsLayout->setSpacing(4);
    m_itemsLayout->setContentsMargins(0, 0, 0, 0);
    m_itemsLayout->setAlignment(Qt::AlignTop); 

    scrollArea->setWidget(itemsContainer);
    m_layout->addWidget(scrollArea, 1);
}

void AnnotationPanel::createItem(const Annotation& annotation, bool checkOrder)
{
    AnnotationWidget* item = new AnnotationWidget(this, annotation);

    connect(item, &AnnotationWidget::removeAnnotationRequested, this, &AnnotationPanel::removeAnnotationRequested);
    connect(item, &AnnotationWidget::updateAnnotationRequested, this, &AnnotationPanel::updateAnnotationRequested);
    connect(item, &AnnotationWidget::annotationClicked, this, &AnnotationPanel::annotationClicked);
    connect(item, &AnnotationWidget::thumbnailRequested, this, &AnnotationPanel::onThumbnailRequested);

    item->refreshThumbnail(); // demande initiale, apres le connect pour ne pas perdre le signal

    if(checkOrder){
        // get first item where start > new annot end
        auto upperBound = std::upper_bound(m_items.cbegin(), m_items.cend(), annotation.end, [](int64_t annotEnd, AnnotationWidget* annotItem){
            return annotEnd < annotItem->annot().start;
        });

        int index = upperBound - m_items.cbegin();
        m_items.insert(index, item);
        m_itemsLayout->insertWidget(index, item);
    }else {
        m_items.append(item);
        m_itemsLayout->addWidget(item);
    }

    // if filter by color is on hide if necessary
    if(m_filterByColorBtn->isChecked())
        item->setVisible(m_filterColors.contains(annotation.color));
}

void AnnotationPanel::onAnnotationAdded(Annotation& annotation){
    createItem(annotation, true);
}

void AnnotationPanel::onAnnotationUpdated(const Annotation& annotation){
    auto it = std::find_if(m_items.cbegin(), m_items.cend(), [&annotation](const AnnotationWidget* a){ return a->annotationId() == annotation.id; });
    if (it == m_items.cend()) {
        qDebug() << "[AnnotationPanel] onAnnotationUpdated : could not find annotation widget with id " << annotation.id;
        return;
    }

    AnnotationWidget* item = *it;
    item->updateAnnotation(annotation);

    // if filter by color is on hide if necessary
    if(m_filterByColorBtn->isChecked())
        item->setVisible(m_filterColors.contains(annotation.color));

    // reposition the item to keep the list chronologically ordered
    int oldIndex = it - m_items.cbegin();
    m_items.remove(oldIndex);

    auto upperBound = std::upper_bound(m_items.cbegin(), m_items.cend(), annotation.end, [](int64_t annotEnd, AnnotationWidget* annotItem){
        return annotEnd < annotItem->annot().start;
    });

    int newIndex = upperBound - m_items.cbegin();
    m_items.insert(newIndex, item);

    // only updates the widget when index changed after reposition
    if(newIndex != oldIndex){
        m_itemsLayout->removeWidget(item);
        m_itemsLayout->insertWidget(newIndex, item);
    }
}

void AnnotationPanel::onAnnotationRemoved(int annotationId){

    auto it = std::find_if(m_items.cbegin(), m_items.cend(), [annotationId](const AnnotationWidget* a){ return a->annotationId() == annotationId; });
    if (it == m_items.cend()) {
        qDebug() << "[AnnotationPanel] onAnnotationRemoved : could not find annotation widget with id " << annotationId;
        return;
    }

    (*it)->deleteLater();
    m_items.erase(it);
}

void AnnotationPanel::rebuild(){
    const QVector<Annotation>& annotations = ProjectManager::instance().annotationManager()->annotations();

    for(int IAnnotItem = 0; IAnnotItem < m_items.size(); ++IAnnotItem){
        m_items[IAnnotItem]->deleteLater();
    }

    m_items.clear();

    // do not check chronological order while rebuilding as annotations should be ordered
    for(auto&& annotation : annotations){
        createItem(annotation, false);
    }
}

void AnnotationPanel::annotationCreationDialog()
{
    int64_t start = m_timeProvider ? m_timeProvider() : 0;
    AnnotationDialog dialog(start ,this);
    if(dialog.exec() == QDialog::Accepted){
        Annotation annotation = dialog.annotation();
        emit addAnnotationRequested(annotation);
    }
}


void AnnotationPanel::onThumbnailRequested(int annotationId, int64_t startMs, QSize targetSize)
{
    Media* media = ProjectManager::instance().media();
    if (!p_thumbnailWorker || !media || media->type() != MediaType::Video)
        return;

    p_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::Annotation, annotationId, startMs, 0, media->filePath(), targetSize, media->sar());
}

void AnnotationPanel::onThumbnailReady(ThumbnailWorker::Requester requester, int requestId, const QImage& image)
{
    if (requester != ThumbnailWorker::Requester::Annotation)
        return;

    auto it = std::find_if(m_items.cbegin(), m_items.cend(), [requestId](const AnnotationWidget* a){ return a->annotationId() == requestId; });
    if (it != m_items.cend())
        (*it)->setThumbnail(image);
}

void AnnotationPanel::filterBy(const QVector<QColor>& colors)
{
    bool inactive = colors.isEmpty();

    m_filterByColorBtn->setButtonState(!inactive);

    if(inactive){ // filter disabled, show all
        for (auto &&item : m_items)
        {
            item->show();
        }
    }else {
        // hides non corresponding widgets
        for (auto &&item : m_items)
        {
            const Annotation& annot = item->annot();
            item->setVisible( colors.contains(annot.color) );
        }
    }

}