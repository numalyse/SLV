#include "ShortcutTab.h"

#include "PrefManager.h"
#include "Preference/Frames/FormShortcutEditFrame.h"
#include "GenericDialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QJsonArray>
#include <QMessageBox>

static QStringList orderedKeys(const QJsonObject& object)
{
    QStringList keys;
    if (object.contains("_order") && object.value("_order").isArray()) {
        for (const QJsonValue& value : object.value("_order").toArray()) {
            if (value.isString()) {
                keys << value.toString();
            }
        }
    } else {
        for (const QString& key : object.keys()) {
            if (!key.startsWith('_')) {
                keys << key;
            }
        }
    }
    return keys;
}

ShortcutTab::ShortcutTab(QWidget *parent) : BasePreferenceTab("Shortcuts", parent)
{
    auto& prefManager = PrefManager::instance();
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    QWidget* resetWidget = new QWidget(this);
    resetWidget->setFixedHeight(30);
    resetWidget->setContentsMargins(0,0,0,0);
    resetWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QHBoxLayout* resetLayout = new QHBoxLayout;
    resetLayout->setContentsMargins(0,0,0,0);
    resetWidget->setLayout(resetLayout);

    QPushButton* resetShortcutsBtn = new QPushButton(PrefManager::instance().getText("reset"), this);
    resetShortcutsBtn->setToolTip(PrefManager::instance().getText("restore_defaults"));
    resetShortcutsBtn->setFixedHeight(30);
    resetShortcutsBtn->setStyleSheet(R"(
        QPushButton{
            background-color: palette(Window);
            padding: 4px 10px;
            border: none;
            border-radius: 4px
        }
        QPushButton:hover{
            background-color: palette(base);
            padding: 4px 10px;
            border: none;
            border-radius: 4px
        }
        )");
    
    QLabel* tabTitle = new QLabel;
    tabTitle->setText(PrefManager::instance().getText("tab_param_shortcuts"));

    QFont tabTitleFont = tabTitle->font();
    tabTitleFont.setPointSize(12);
    tabTitleFont.setBold(true);
    tabTitle->setFont(tabTitleFont);

    resetLayout->addWidget(tabTitle);
    resetLayout->addStretch();
    resetLayout->addWidget(resetShortcutsBtn);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_containerLayout->insertWidget(0, line);
    m_containerLayout->insertWidget(0, resetWidget);
    
    resetShortcutsBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    connect(resetShortcutsBtn, &QPushButton::clicked, this, &ShortcutTab::AskResetDefault);

    QJsonObject defaultShortcuts = prefManager.getDefaultCategory(m_categoryName);
    QStringList sections = orderedKeys(defaultShortcuts);

    for (const QString& section : sections) {
        if (!m_baseJson.contains(section)) {
            continue;
        }

        m_layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" + section), m_container));
        QJsonObject shortCutSubcategory = m_baseJson.value(section).toObject();
        QJsonObject defaultSection = defaultShortcuts.value(section).toObject();
        QStringList shortcutKeys = orderedKeys(defaultSection);

        for (const QString& internalKey : shortcutKeys) {
            if (!shortCutSubcategory.contains(internalKey)) {
                continue;
            }

            QString keyTranslated = prefManager.getText("shortcut_" + internalKey);
            QString value = shortCutSubcategory.value(internalKey).toString();

            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(
                keyTranslated,
                section,
                internalKey,
                value,
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
            + "\n\n\n" 
            + prefManager.getText("shortcut_subsection_" + conflictSubCat) 
            + " > " + prefManager.getText("shortcut_" + conflictKey)
            + " : " + newValue
            + "\n",
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

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(prefManager.getText("reset"));
    msgBox.setText(prefManager.getText("restore_defaults_asked"));

    QPushButton* confirmBtn = msgBox.addButton(prefManager.getText("confirm"), QMessageBox::YesRole);
    msgBox.addButton(prefManager.getText("generic_dialog_btn_cancel"), QMessageBox::RejectRole);

    msgBox.exec();
    if (msgBox.clickedButton() == confirmBtn) {
        resetDefault();
    }
}

void ShortcutTab::resetDefault(){
    auto& prefManager = PrefManager::instance();

    QJsonObject defaultShortcuts = prefManager.getDefaultCategory(m_categoryName);
    QStringList sections = orderedKeys(defaultShortcuts);

    for (const QString& section : sections) {
        if (!defaultShortcuts.contains(section)) {
            continue;
        }

        QJsonObject defaultSection = defaultShortcuts.value(section).toObject();
        QStringList shortcutKeys = orderedKeys(defaultSection);

        for (const QString& internalKey : shortcutKeys) {
            if (!defaultSection.contains(internalKey)) {
                continue;
            }

            QString defaultValue = defaultSection.value(internalKey).toString();
            BasePreferenceTab::updateJsonObj(section, internalKey, defaultValue);

            if (m_shortcutFrames.contains(internalKey)) {
                m_shortcutFrames[internalKey]->setUIValue(defaultValue);
            }
        }
    }
}