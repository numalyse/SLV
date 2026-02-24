#include "TextManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QDebug>

/// @brief Charge le json de la langue choisi
/// @param langCode "fr","en"
void TextManager::loadLanguage(const QString& langCode) 
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

/// @brief Retrouve la valeur associé à la clé
/// @param key QString
/// @return QString, Si la valeur associé à la clé est trouvé, retourne la valeur, sinon retourne la clé
QString TextManager::get(const QString &key) const
{
    if (m_texts.contains(key)) {
        return m_texts[key].toString();
    }
    qDebug() << key << " n'est pas dans le fichier json";
    return "[" + key + "]";
}
