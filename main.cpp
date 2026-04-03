#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(
        "ToolbarButton, ToolbarToggleButton, #durationToggleBtn {"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "ToolbarButton:hover, ToolbarToggleButton:hover, #durationToggleBtn:hover {"
        "   background-color: palette(button);"
        "   border: 1px solid palette(button);"
        "   border-radius: 4px;"
        "}"
        "#durationToggleBtn {"
        "   padding: 3px 6px;" 
        "}"

    );

    MainWindow w;
    w.show();

    return app.exec();
}
