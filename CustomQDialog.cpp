#include "CustomQDialog.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QStyleHints>
#include <QGuiApplication>
#include <QDir>
#include <QProgressDialog>
#include <QProcess>

CustomQDialog::CustomQDialog(
    const QString& titre,
    const QString& texte,
    const QString& bouton1,
    const QString& bouton2,
    const QString& path,
    QWidget* parent
)
    : QDialog(parent)
    , m_titre(titre)
    , m_texte(texte)
    , m_bouton1(bouton1)
    , m_bouton2(bouton2)
    , m_path(path)
{
    setupUi();
    setupColors();
    setupStyle();
}

void CustomQDialog::setupUi()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(500, 220);

    setObjectName("dialogWindow");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);


    auto* container = new QWidget(this);
    container->setObjectName("dialogContainer");

    outerLayout->addWidget(container);

    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    m_titleLabel = new QLabel(m_titre, container);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);

    m_titleLabel->setFont(titleFont);

    layout->addWidget(m_titleLabel);

    m_textLabel = new QLabel(m_texte, container);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setWordWrap(true);

    QFont textFont = m_textLabel->font();
    textFont.setPointSize(10);

    m_textLabel->setFont(textFont);

    layout->addWidget(m_textLabel);

    auto* btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(20);

    // Bouton 1 : Fermer/Ok
    m_button1 = new QPushButton(m_bouton1, container);

    // Bouton 2 : Si existe
    if (!m_bouton2.isEmpty())
    {
        m_button2 = new QPushButton(m_bouton2, container);
    }

    QFont btnFont = m_button1->font();
    btnFont.setPointSize(10);
    btnFont.setBold(true);

    m_button1->setFont(btnFont);

    if (m_button2)
        m_button2->setFont(btnFont);

    if (m_button2)
    {
        m_button1->setFixedSize(200, 40);
        m_button2->setFixedSize(200, 40);
    }
    else
    {
        m_button1->setFixedSize(200, 40);
    }

    if (m_button2)
        btnLayout->addWidget(m_button2);

    btnLayout->addWidget(m_button1);

    layout->addLayout(btnLayout);

    connect(
        m_button1,
        &QPushButton::clicked,
        this,
        &CustomQDialog::onCloseClicked
    );

    if (m_button2)
    {
        connect(
            m_button2,
            &QPushButton::clicked,
            this,
            &CustomQDialog::onPathClicked
        );
    }
}

void CustomQDialog::setupColors()
{
    m_isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

#ifdef Q_OS_MAC
    QColor colorBtn = qApp->palette().color(QPalette::Button);
    QColor enhancedColor = m_isDarkMode ? colorBtn.lighter(150) : colorBtn.darker(150);
    m_palbtnColor = m_isDarkMode ? enhancedColor : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? QString(enhancedColor.name()) : "palette(mid)";
#else
    m_palbtnColor = m_isDarkMode ? "palette(button)" : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? "palette(button)" : "palette(mid)";
#endif

}

void CustomQDialog::setupStyle()
{

    const QString color = m_palbtnColorStr;

    setStyleSheet(
        "#dialogWindow {"
            "background-color: transparent;"
        "}"

        "#dialogContainer {"
            "background-color: palette(Window);"
            "border: 2px solid " + color + ";"
            "border-radius: 20px;"
        "}"
    );

    if (m_button2)
    {
        m_button2->setStyleSheet(
            "QPushButton {"
                "background-color: " + color + ";"
                "border: 1px solid " + color + ";"
                "border-radius: 4px;"
            "}"

            "QPushButton:hover {"
                "background-color: palette(Window);"
                "border: 2px solid " + color + ";"
                "border-radius: 4px;"
            "}"
        );
    }

    m_button1->setStyleSheet(
                "QPushButton {"
                    "background-color: palette(Window);"
                    "border: 1px solid " + color + ";"
                    "border-radius: 4px;"
                "}"
                "QPushButton:hover {"
                    "background-color: " + m_palbtnColorStr + ";"
                    "border: 2px solid " + color + ";"
                    "border-radius: 4px;"
                "}"
            );

}

void CustomQDialog::onCloseClicked()
{
    accept();
}

void CustomQDialog::onPathClicked()
{
    if (m_path.isEmpty())
        return;

    QFileInfo fi(m_path);
#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(fi.absoluteFilePath());
    QProcess::startDetached("explorer.exe", args);
#elif defined(Q_OS_MACOS)
    QProcess::startDetached("open", QStringList() << "-R" << fi.absoluteFilePath());
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
#endif

    accept();

}