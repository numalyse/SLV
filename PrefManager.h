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

    /// @brief Load the file in SLV Content if present, if not present creates a new one before loading
    void loadUserPrefs();

    /// @brief Gets an entire subcategory
    /// retrieves the default subcategory then adds user prefs, also checks is user pref key is in default pref. Prevents user adding random keys
    QJsonObject getCategory(const QString &category) const;

    /// @brief Gets an entire category
    /// retrieves the default category then adds user prefs, also checks is user pref key is in default pref. Prevents user adding random keys
    QJsonObject getSubCategory(const QString &category, const QString &subCategory) const;

    /// @brief Finds the value of the specified key in the specified category
    /// @return If key was found, returns its value otherwise "[key]"
    QString getPref(const QString &category, const QString &key) const;
    QString getPref(const QString &category, const QString &subCategory, const QString &key) const;

    /// @brief Writes directly to the json file the value of the key
    /// @return True if successfully written to, false otherwise 
    bool setPref(const QString &category, const QString &key, const QString &value);

    /// @brief Write all the data of m_userPrefs in the json file
    /// @return True if successfully written to, false otherwise  
    bool writeUserJson();

private:
    /// @brief Creates the preference file in SLV Contents
    /// @return True if successfully created, false otherwise 
    bool createPreferenceFile(const QString &filePath);

    /// @brief Recursive function that traverses the JSON object and adds any missing default keys to user keys.
    /// @param defaultObj The reference JSON object containing the default preferences.
    /// @param userObj The user's JSON object that needs to be updated with missing keys.
    /// @return True if 'userObj' was modified (missing keys were added), false otherwise.
    bool mergeMissingKeys(const QJsonObject& defaultObj, QJsonObject& userObj);


    /// @brief Checks if the user pref are missing keys from default prefs,
    /// sets the paths if they are empty and writes to the json if m_userPrefs was modified 
    void syncUserPrefs();

    PrefManager() {} ;

    QJsonObject m_texts;
    QJsonObject m_userPrefs;
    QJsonObject m_defaultPrefs;
};

#endif