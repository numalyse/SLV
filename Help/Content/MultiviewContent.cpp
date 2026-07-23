#include "MultiviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MultiviewContent::MultiviewContent(QWidget* parent)
    : CategoryBase("help_menu_multiview_category", parent)
{
    QString multiview = "help_menu_enter_multiview_label";

    addSubcategory(
        multiview,
        introduction(multiview)
    );

    
}

QWidget* MultiviewContent::introduction(const QString& subcategoryName)
{
    QWidget* widget = new ContentBase(this, categoryName(), subcategoryName);

    return widget;
}
