#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <QColor>
#include <QString>

class UserConfig
{
public:
    enum class OperatingSystem
    {
        Windows,
        MacOS,
        Linux,
        Unknown
    };

    static UserConfig& instance()
    {
        static UserConfig config;
        return config;
    }

    void initialize();

    OperatingSystem operatingSystem() const;
    bool isDarkMode() const;
    QColor paletteButtonColor() const;
    QString paletteButtonColorString() const;
    QString mainControlKeyString() const;

private:
    UserConfig() = default;

    OperatingSystem m_operatingSystem = OperatingSystem::Unknown;
    bool m_isDarkMode = true;
    QColor m_paletteButtonColor;
    QString m_paletteButtonColorString;
    QString m_mainControlKeyString;
};

#endif // USERCONFIG_H
