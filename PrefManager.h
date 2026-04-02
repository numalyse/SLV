#ifndef PrefManager_H
#define PrefManager_H

#include <QString>
#include <QJsonObject>

/// @brief Singleton pour gérer les différentes langues
class PrefManager {

public:

    static PrefManager& instance() {
        static PrefManager _instance;
        return _instance;
    }

    void loadLanguage(const QString& langCode);
    QString getText(const QString &key) const;

    void loadPrefs(const QString& langCode);
    QString getPref(const QString &key) const;

private:
    PrefManager() {} ;
    QJsonObject m_texts;
    QJsonObject m_prefs;
};

#endif