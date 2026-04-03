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

    /// @brief Load the json of the specified langCode
    /// @param langCode "fr", "en"
    void loadLanguage(const QString& langCode);

    /// @brief Finds the value of the specified key
    /// @return If key was found, returns its value otherwise "[key]"
    QString getText(const QString &key) const;

    ///@brief Loads both user prefs and default prefs
    void loadPrefs();

    /// @brief Loads the default preference to be used as fallback if key was not found in user pref
    void loadDefaultPrefs();

    void loadUserPrefs();

    QString getPref(const QString &key) const;

private:
    /// @brief Creates the preference file in SLV Contents
    /// @return True if successfully created false otherwise 
    bool createPreferenceFile(const QString &filePath);



    PrefManager() {} ;

    QJsonObject m_texts;
    QJsonObject m_userPrefs;
    QJsonObject m_defaultPrefs;
};

#endif