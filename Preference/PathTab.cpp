#include "PathTab.h"

#include "PrefManager.h"

PathTab::PathTab(QWidget *parent) : BasePreferenceTab("Paths", parent)
{
    auto& prefManager = PrefManager::instance();
    
    FormPathEditFrame* projectPathFrame = new FormPathEditFrame(
        prefManager.getText("paths_screenshot"),          
        "Paths",                      
        "screenshot",           
        prefManager.getPref("Paths", "screenshot"),
        true,                           
        this
    );

    addPreferenceFrame( projectPathFrame );
}