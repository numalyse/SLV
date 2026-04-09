#include "ShortcutTab.h"

#include "PrefManager.h"
#include "Preference/Frames/FormShortcutEditFrame.h"
#include "GenericDialog.h"


#include <QFormLayout>
#include <QLabel>

ShortcutTab::ShortcutTab(QWidget *parent) : BasePreferenceTab("Shortcuts", parent)
{
    auto& prefManager = PrefManager::instance();
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    for (auto IsubCategory = m_baseJson.begin(); IsubCategory != m_baseJson.end(); ++IsubCategory) {
        
        m_layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" +  IsubCategory.key()), m_container));
        QJsonObject shortCutSubcategory = IsubCategory.value().toObject();

        for(auto IKey = shortCutSubcategory.begin(); IKey != shortCutSubcategory.end(); ++IKey){
            QString internalKey = IKey.key();
            QString keyTranslated = prefManager.getText("shortcut_" + internalKey);
    
            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(
                keyTranslated, 
                IsubCategory.key(), 
                internalKey, 
                IKey.value().toString(), 
                m_container
            );
            
            connect(formShortcutEditFrame, &FormShortcutEditFrame::emptyShortcutUIRequested, this, &ShortcutTab::emptyShortcutUI);
            addPreferenceFrame(formShortcutEditFrame);

            m_shortcutFrames.insert(internalKey, formShortcutEditFrame);
        }

    }
}

void ShortcutTab::updateJsonObj(const QString& subCategory, const QString& key, const QString& newValue)
{
    QString conflictSubCat, conflictKey;

    if (hasConflict(newValue, key, conflictSubCat, conflictKey)) {
        auto& prefManager = PrefManager::instance();
        
        SLV::showGenericDialog(
            this, 
            prefManager.getText("dialog_update_shortcut_conflict_title"),
            prefManager.getText("dialog_update_shortcut_conflict_text") 
            + prefManager.getText("shortcut_subsection_" + conflictSubCat) 
            + ", " + prefManager.getText("shortcut_" + conflictKey)
            + " : " + newValue,
            prefManager.getText("dialog_update_shortcut_conflict_text_info"),
            
            [this, subCategory, key, newValue, conflictSubCat, conflictKey]() { 
                BasePreferenceTab::updateJsonObj(subCategory, key, newValue);
                BasePreferenceTab::updateJsonObj(conflictSubCat, conflictKey, "");
                m_shortcutFrames[conflictKey]->setUIValue(""); 
                m_shortcutFrames[key]->setUIValue(newValue); 
            },
            
            nullptr,
            
            [this, subCategory, key]() {
                QString prevVal = m_updatedJson[subCategory].toObject()[key].toString();
                m_shortcutFrames[key]->setUIValue(prevVal);
            }
        );
    } else {
        BasePreferenceTab::updateJsonObj(subCategory, key, newValue);
        m_shortcutFrames[key]->setUIValue(newValue);
    }
}

bool ShortcutTab::hasConflict(const QString &newShortcut, const QString &currentKey, QString &outSubCategory, QString &outKey)
{
    if (newShortcut.isEmpty()) return false;
    for (auto IsubCategory = m_updatedJson.begin(); IsubCategory != m_updatedJson.end(); ++IsubCategory) {
        QJsonObject subCatObj = IsubCategory.value().toObject();
        
        for (auto IKey = subCatObj.begin(); IKey != subCatObj.end(); ++IKey) {
            if (IKey.key() != currentKey && IKey.value().toString() == newShortcut) {
                outSubCategory = IsubCategory.key();
                outKey = IKey.key();
                return true;
            }
        }
    }
    return false;
}

void ShortcutTab::emptyShortcutUI(const QString& stolenKey)
{
    if(m_shortcutFrames.contains(stolenKey)) {
        m_shortcutFrames[stolenKey]->clearShortcutUI();
    }
}
