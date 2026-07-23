#include "MultiviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MultiviewContent::MultiviewContent(QWidget* parent)
    : CategoryBase("Multiview", parent)
{

    addSubcategory(
        "Introduction",
        introduction()
    );

    
}

QWidget* MultiviewContent::introduction()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Bienvenue dans la documentation!!!!"));

    return widget;
}
