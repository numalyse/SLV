#include "MonoviewContent.h"

#include "PrefManager.h"
#include "../Base/ContentBase.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

MonoviewContent::MonoviewContent(QWidget* parent)
    : CategoryBase("Monoview", parent)
{

    addSubcategory(
        "Introduction",
        introduction()
    );

    addSubcategory(
        "Player",
        player()
    );

    
}

QWidget* MonoviewContent::introduction()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Bienvenue dans la documentation!!!!"));

    return widget;
}

QWidget* MonoviewContent::player()
{
    QWidget* widget = new QWidget;

    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("Test"));

    return widget;
}