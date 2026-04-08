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


    m_baseShortcut = prefManager.getCategory("Shortcuts");
    m_updatedShortcut = m_baseShortcut;
    

    for (auto category = m_baseShortcut.begin(); category != m_baseShortcut.end(); ++category) {
        
        layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" +  category.key()), container));
        QJsonObject shortCutSubcategory = category.value().toObject();

        for(auto subCategory = shortCutSubcategory.begin(); subCategory != shortCutSubcategory.end(); ++subCategory){
            QString internalKey = subCategory.key();
            QString keyTranslated = prefManager.getText("shortcut_" + internalKey);
    
            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(keyTranslated, category.key(), internalKey, subCategory.value().toString(), container);
            
            m_shortcutFrames.insert(internalKey, formShortcutEditFrame);
            connect(formShortcutEditFrame, &FormShortcutEditFrame::updateJsonObjRequested, this, &ShortcutTab::updateJsonObj);
            connect(formShortcutEditFrame, &FormShortcutEditFrame::emptyShortcutUIRequested, this, &ShortcutTab::emptyShortcutUI);
            layout->addWidget(formShortcutEditFrame);
        }

    }

    setWidget(container);

}

void ShortcutTab::updateJsonObj(const QString& subCategory, const QString& key, const QString& newShortcutString)
{
    QJsonObject subObject = m_updatedShortcut[subCategory].toObject();
    subObject[key] = newShortcutString;
    m_updatedShortcut[subCategory] = subObject;
}

bool ShortcutTab::needSave(){
    return m_baseShortcut != m_updatedShortcut;
}

void ShortcutTab::emptyShortcutUI(const QString& stolenKey)
{
    if(m_shortcutFrames.contains(stolenKey)) {
        m_shortcutFrames[stolenKey]->clearShortcutUI();
    }
}

void ShortcutTab::save(){
    auto& prefManager = PrefManager::instance();
    if( prefManager.setCategory("Shortcuts", m_updatedShortcut)) {
        m_baseShortcut = m_updatedShortcut;
    } else {
        qWarning() << "[ShortcutTab] Echec de la sauvegarde des raccourcis";
    }
}