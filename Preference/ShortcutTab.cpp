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
        layout->addRow(new QLabel(prefManager.getText("shortcut_subsection_" +  category.key()), this));
        QJsonObject shortCutSubcategory = category.value().toObject();
        for(auto subCategory = shortCutSubcategory.begin(); subCategory != shortCutSubcategory.end(); ++subCategory){
            QString keyTranslated = prefManager.getText("shortcut_" + subCategory.key());
            
            FormShortcutEditFrame* formShortcutEditFrame = new FormShortcutEditFrame(keyTranslated, "MainWindow", subCategory.key(), subCategory.value().toString(), this);
            layout->addWidget(formShortcutEditFrame);
        }

    }

    setWidget(container);


}