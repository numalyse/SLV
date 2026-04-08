#include "ShortcutTab.h"

#include "PrefManager.h"
#include "Preference/FormShortcutEditFrame.h"


#include <QFormLayout>
#include <QLabel>

ShortcutTab::ShortcutTab(QWidget *parent) : BasePreferenceTab("Shortcuts", parent)
{
    auto& prefManager = PrefManager::instance();
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    for (auto category = m_baseJson.begin(); category != m_baseJson.end(); ++category) {
        
        m_layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" +  category.key()), m_container));
        QJsonObject shortCutSubcategory = category.value().toObject();

        for(auto subCategory = shortCutSubcategory.begin(); subCategory != shortCutSubcategory.end(); ++subCategory){
            QString internalKey = subCategory.key();
            QString keyTranslated = prefManager.getText("shortcut_" + internalKey);
    
            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(
                keyTranslated, 
                category.key(), 
                internalKey, 
                subCategory.value().toString(), 
                m_container
            );
            
            connect(formShortcutEditFrame, &FormShortcutEditFrame::emptyShortcutUIRequested, this, &ShortcutTab::emptyShortcutUI);
            addPreferenceFrame(formShortcutEditFrame);

            m_shortcutFrames.insert(internalKey, formShortcutEditFrame);
        }

    }
}

void ShortcutTab::emptyShortcutUI(const QString& stolenKey)
{
    if(m_shortcutFrames.contains(stolenKey)) {
        m_shortcutFrames[stolenKey]->clearShortcutUI();
    }
}
