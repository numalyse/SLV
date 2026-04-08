#include "ShortcutTab.h"

#include "PrefManager.h"
#include "Preference/FormShortcutEditFrame.h"

#include <QFormLayout>
#include <QLabel>

ShortcutTab::ShortcutTab(QWidget *parent) : QScrollArea(parent)
{
    auto& prefManager = PrefManager::instance();
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(this);
    QFormLayout* layout = new QFormLayout(container);


    QJsonObject shortCuts = prefManager.getCategory("Shortcuts");

    for (auto category = shortCuts.begin(); category != shortCuts.end(); ++category) {
        
        layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" +  category.key()), container));
        QJsonObject shortCutSubcategory = category.value().toObject();

        for(auto subCategory = shortCutSubcategory.begin(); subCategory != shortCutSubcategory.end(); ++subCategory){
            QString internalKey = subCategory.key();
            QString keyTranslated = prefManager.getText("shortcut_" + internalKey);
    
            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(keyTranslated, category.key(), internalKey, subCategory.value().toString(), container);
            
            connect(formShortcutEditFrame, &FormShortcutEditFrame::emptyShortcutOf, this, &ShortcutTab::onShortcutStolen);
            
            m_shortcutFrames.insert(internalKey, formShortcutEditFrame);
            layout->addWidget(formShortcutEditFrame);
        }

    }

    setWidget(container);

}


void ShortcutTab::onShortcutStolen(const QString& stolenKey)
{
    if(m_shortcutFrames.contains(stolenKey)) {
        m_shortcutFrames[stolenKey]->clearShortcutUI();
    }
}