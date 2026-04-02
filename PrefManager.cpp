#include "PrefManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QDebug>

/// @brief Charge le JSON de la langue choisie
/// @param langCode "fr", "en"
void PrefManager::loadLanguage(const QString& langCode) 
{
    QString filePath = ":/lang/" + langCode + ".json";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_texts = doc.object();
        file.close();
    } else {
        qDebug() << "Impossible de charger le fichier de langue:" << filePath;
    }
}

/// @brief Retrouve la valeur associée à la clé
/// @param key QString
/// @return QString. Si la valeur associée à la clé est trouvée, retourne la valeur, sinon retourne la clé.
QString PrefManager::getText(const QString &key) const
{
    if (m_texts.contains(key)) {
        return m_texts[key].toString();
    }
    qDebug() << key << " n'est pas dans le fichier json";
    return "[" + key + "]";
}
