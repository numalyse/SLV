#include "AnnotationPanel.h"
#include "AnnotationDialog.h"
#include "IconHelper.h"
#include "Project/ProjectManager.h"

#include <algorithm>

AnnotationPanel::AnnotationPanel(QWidget *parent) : QWidget(parent)
{
    auto* annotations = ProjectManager::instance().annotationManager();

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

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_addAnnotationBtn = new ToolbarButton(this, "plus_white");
    connect(m_addAnnotationBtn, &QPushButton::clicked, this, &AnnotationPanel::annotationCreationDialog);

    // color buttons shown in the popup when hovering the filter button
    QHBoxLayout* filterColorsLayout = new QHBoxLayout();
    for (const auto& [colorName, color] : IconHelper::colorPalette()) {
        ToolbarButton* colorBtn = new ToolbarButton(this, " ", colorName);
        colorBtn->setIcon(IconHelper::genIconPreviewColor(color, color == QColor{255, 255, 255, 255}));
        connect(colorBtn, &ToolbarButton::clicked, this, [this, color](){ filterBy(color); });
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
        if (m_filterColor.isValid())
            filterBy(m_filterColor);
        else
            m_filterByColorBtn->setButtonState(false); // no color picked yet, nothing to filter
    });
    connect(m_filterByColorBtn, &ToolbarToggleButton::stateDeactivated, this, [this](){
        filterBy(QColor());
    });

    buttonLayout->addWidget(m_addAnnotationBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_filterByColorBtn);

    m_layout->addLayout(buttonLayout);

    m_itemsLayout = new QVBoxLayout();
    m_itemsLayout->setSpacing(4);
    m_layout->addLayout(m_itemsLayout);

    m_layout->addStretch();
}

void AnnotationPanel::createItem(const Annotation& annotation, bool checkOrder)
{
    AnnotationWidget* item = new AnnotationWidget(this, annotation);

    connect(item, &AnnotationWidget::removeAnnotationRequested, this, &AnnotationPanel::removeAnnotationRequested);
    connect(item, &AnnotationWidget::updateAnnotationRequested, this, &AnnotationPanel::updateAnnotationRequested);
    connect(item, &AnnotationWidget::annotationClicked, this, &AnnotationPanel::annotationClicked);

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
        item->setVisible(annotation.color == m_filterColor);
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
        item->setVisible(annotation.color == m_filterColor);

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
    //int64_t end = 
    AnnotationDialog dialog(this, {});
    if(dialog.exec() == QDialog::Accepted){
        Annotation annotation = dialog.annotation();
        emit addAnnotationRequested(annotation);
    }
}


void AnnotationPanel::filterBy(const QColor& color)
{
    bool active = color.isValid();
    if(active) // remembered so the toggle button can restore the last filter
        m_filterColor = color;

    m_filterByColorBtn->setButtonState(active);

    if(!active){ // filter disabled, show all
        m_filterByColorBtn->setStyleSheet("");
        for (auto &&item : m_items)
        {
            item->show();
        }
    }else {
        // updates the border color of m_filterByColorBtn and hides non corresponding widgets
        m_filterByColorBtn->setStyleSheet(
            QString("border: 2px solid %1; border-radius: 4px;").arg(color.name()));
        for (auto &&item : m_items)
        {
            const Annotation& annot = item->annot();
            item->setVisible(annot.color == color);
        }
    }

}