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

    QPushButton* resetShortcuts = new QPushButton(PrefManager::instance().getText("reset"), this);
    resetShortcuts->setToolTip(PrefManager::instance().getText("restore_defaults"));
    m_layout->addWidget(resetShortcuts);
    connect(resetShortcuts, &QPushButton::clicked, this, &ShortcutTab::AskResetDefault);

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

    if (hasConflict(newValue, key, conflictSubCat, conflictKey)) { // if has conflict shows the dialog to override the value
        auto& prefManager = PrefManager::instance();
        
        SLV::showGenericDialog(
            this, 
            prefManager.getText("dialog_update_shortcut_conflict_title"),
            prefManager.getText("dialog_update_shortcut_conflict_text") 
            + prefManager.getText("shortcut_subsection_" + conflictSubCat) 
            + ", " + prefManager.getText("shortcut_" + conflictKey)
            + " : " + newValue,
            prefManager.getText("dialog_update_shortcut_conflict_text_info"),
            
            [this, subCategory, key, newValue, conflictSubCat, conflictKey]() { // chose to override, set the value and empties the other
                BasePreferenceTab::updateJsonObj(subCategory, key, newValue);
                BasePreferenceTab::updateJsonObj(conflictSubCat, conflictKey, "");
                m_shortcutFrames[conflictKey]->setUIValue(""); 
                m_shortcutFrames[key]->setUIValue(newValue); 
            },
            
            nullptr,
            
            [this, subCategory, key]() { // on cancel revert the shortcut back to its original value
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

void ShortcutTab::AskResetDefault(){
    auto& prefManager = PrefManager::instance();

    SLV::showGenericDialog(
        this,
        prefManager.getText("reset"),
        prefManager.getText("restore_defaults"),
        [this]() { resetDefault(); },
        nullptr,
        nullptr
    );
}

void ShortcutTab::resetDefault(){
    auto& prefManager = PrefManager::instance();

    // Retrieve the default category (no user overrides)
    QJsonObject defaultCat = prefManager.getDefaultCategory(m_categoryName);

    for (auto IsubCategory = defaultCat.begin(); IsubCategory != defaultCat.end(); ++IsubCategory) {
        if (!IsubCategory.value().isObject()) continue;

        QJsonObject subObj = IsubCategory.value().toObject();

        for (auto IKey = subObj.begin(); IKey != subObj.end(); ++IKey) {
            QString internalKey = IKey.key();
            QString defaultValue = IKey.value().toString();

            // Bypass conflict dialog when restoring defaults by using BasePreferenceTab implementation
            BasePreferenceTab::updateJsonObj(IsubCategory.key(), internalKey, defaultValue);

            if (m_shortcutFrames.contains(internalKey)) {
                m_shortcutFrames[internalKey]->setUIValue(defaultValue);
            }
        }
    }
}