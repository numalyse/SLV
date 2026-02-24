#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <QString>
#include <QJsonObject>

/// @brief Singleton pour gérer les différentes langues
class TextManager {

public:

    static TextManager& instance() {
        static TextManager _instance;
        return _instance;
    }

    void loadLanguage(const QString& langCode);
    QString get(const QString &key) const;

private:
    TextManager() {} ;
    QJsonObject m_texts;
};

#endif