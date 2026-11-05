#include "UserConfig.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>
#include <QKeySequence>

void UserConfig::initialize()
{
#if defined(Q_OS_WIN)
    m_operatingSystem = OperatingSystem::Windows;
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    m_operatingSystem = OperatingSystem::MacOS;
#elif defined(Q_OS_LINUX)
    m_operatingSystem = OperatingSystem::Linux;
#else
    m_operatingSystem = OperatingSystem::Unknown;
#endif

    m_isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

#ifdef Q_OS_MAC
    const QColor buttonColor = qApp->palette().color(QPalette::Button);
    const QColor enhancedColor = m_isDarkMode ? buttonColor.lighter(150) : buttonColor.darker(150);
    m_paletteButtonColor = m_isDarkMode ? enhancedColor : QColor("palette(mid)");
    m_paletteButtonColorString = m_isDarkMode ? enhancedColor.name() : "palette(mid)";
#else
    m_paletteButtonColor = QColor(m_isDarkMode ? "palette(button)" : "palette(mid)");
    m_paletteButtonColorString = m_isDarkMode ? "palette(button)" : "palette(mid)";
#endif

    m_mainControlKeyString = QKeySequence(QKeyCombination(Qt::CTRL | Qt::Key_A))
                                 .toString(QKeySequence::NativeText);
    const QString nativeAKey = QKeySequence(Qt::Key_A).toString(QKeySequence::NativeText);
    if (m_mainControlKeyString.endsWith(nativeAKey))
    {
        m_mainControlKeyString.chop(nativeAKey.size());
        if (m_mainControlKeyString.endsWith(QLatin1Char('+'))){
            m_mainControlKeyString.chop(1);
        }
    }
}

UserConfig::OperatingSystem UserConfig::operatingSystem() const
{
    return m_operatingSystem;
}

bool UserConfig::isDarkMode() const
{
    return m_isDarkMode;
}

QColor UserConfig::paletteButtonColor() const
{
    return m_paletteButtonColor;
}

QString UserConfig::paletteButtonColorString() const
{
    return m_paletteButtonColorString;
}

QString UserConfig::mainControlKeyString() const
{
    return m_mainControlKeyString;
}


