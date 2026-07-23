#include "mainwindow.h"

#include <QApplication>
#include <QStyleHints>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QString checkIcon = isDarkMode ? ":/icons/check_white" : ":/icons/check";

#if defined(Q_OS_MAC)

    QString ffmpegPath = QCoreApplication::applicationDirPath() + "/../Resources/ffmpeg/ffmpeg";
    QProcess::execute("xattr", {"-dr", "com.apple.quarantine", ffmpegPath});

    app.setStyleSheet(
        "ToolbarButton, ToolbarToggleButton, #durationToggleBtn {"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "#durationToggleBtn {"
        "   padding: 3px 6px;"
        "}"
        "QRadioButton{"
        "   padding: 3px 6px;"
        "}"
        "QRadioButton::indicator{"
        "   height: 20px;"
        "   width: 20px;"
        "}"
        "QRadioButton::indicator:unchecked {"
        "   image: url();"
        "}"
        "QRadioButton::indicator:checked {"
        "   image: url(" + checkIcon + ");"
        "}"
    );

    QColor colorBtn = qApp->palette().color(QPalette::Button);
    QColor enhancedColor = colorBtn.lighter(150); // Adjust the factor as needed
    QString enhancedColorString= enhancedColor.name();

    QString hoverString = QString(
                      "ToolbarButton:hover, ToolbarToggleButton:hover, #durationToggleBtn:hover {"
                      "   background-color: %1;"
                      "   border: 1px solid %1;"
                      "   border-radius: 4px;"
                      "}"
                      "QRadioButton:hover{"
                      "   background-color: %1;"
                      "}").arg(enhancedColorString);

    app.setStyleSheet(app.styleSheet() + hoverString);

#else


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
        // "ToolbarButton:disabled, ToolbarToggleButton:disabled, #durationToggleBtn:disabled {"
        // "   border: none;"
        // "}"
        "#durationToggleBtn {"
        "   padding: 3px 6px;"
        "}"
        "QRadioButton{"
        "   padding: 3px 6px;"
        "}"
        "QRadioButton::indicator{"
        "   height: 20px;"
        "   width: 20px;"
        "}"
        "QRadioButton:hover{"
        "   background-color: palette(button);"
        "}"
        "QRadioButton::indicator:unchecked {"
        "   image: url();"
        "}"
        "QRadioButton::indicator:checked {"
        "   image: url(" + checkIcon + ");"
        "}"

    );
#endif
    

    MainWindow w;
    w.show();

    return app.exec();
}
