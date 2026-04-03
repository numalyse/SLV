#include "PrefManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>


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
        qDebug() << "[PrefManager] Impossible de charger le fichier de langue:" << filePath;
    }
}


QString PrefManager::getText(const QString &key) const
{
    if (m_texts.contains(key)) {
        return m_texts[key].toString();
    }
    qDebug() << "[PrefManager] " << key << " n'est pas dans le fichier json";
    return "[" + key + "]";
}


bool PrefManager::createPreferenceFile(const QString &destFilePath)
{
    QFileInfo fileInfo(destFilePath);
    QDir dir = fileInfo.absoluteDir();

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "[PrefManager] Impossible de créer le dossier SLV Contents pour les préférences.";
            return false;
        }
    }

    QString resourcePath = ":/defaultPref.json";
    QFile defaultFile(resourcePath);

    if (defaultFile.copy(destFilePath)) {
        QFile::setPermissions(destFilePath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser);
        return true;
    }

    return false;
}

void PrefManager::loadDefaultPrefs(){
    QString resourcePath = ":/defaultPref.json";
    QFile defaultFile(resourcePath);

    if (defaultFile.open(QIODevice::ReadOnly)) {
        QByteArray data = defaultFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_defaultPrefs = doc.object();
        defaultFile.close();
    }else {
        qCritical() << "[PrefManager] Impossible d'ouvrir le fichier des préférences par défaut : " << defaultFile.errorString();
    }

}

void PrefManager::loadUserPrefs(){

    QString filePath = QDir(QDir::homePath()).filePath("SLV_Content/pref.json");
    QFile file(filePath);

    if ( ! file.exists() ){
        if(!createPreferenceFile(filePath)){
            qCritical() << "[PrefManager] Impossible de copier le fichier des preferences dans SLV contents";
            return; 
        } 
    }

    if (file.open(QIODevice::ReadWrite)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_userPrefs = doc.object();
        file.close();
    } else {
        qCritical() << "[PrefManager] Impossible d'ouvrir le fichier des préférences : " << file.errorString();
    }
}

void PrefManager::loadPrefs()
{
    loadDefaultPrefs();
    loadUserPrefs();
}


QString PrefManager::getPref(const QString &category, const QString &key) const
{
    if (m_userPrefs.contains(category)) {
        QJsonObject userCatObj = m_userPrefs.value(category).toObject();
        if (userCatObj.contains(key)) {
            return userCatObj.value(key).toString();
        }
    }
    if (m_defaultPrefs.contains(category)) {
        QJsonObject defaultCatObj = m_defaultPrefs.value(category).toObject();
        
        if (defaultCatObj.contains(key)) {
            return defaultCatObj.value(key).toString();
        }
    }

    qWarning() << "La clé" << key << "dans la catégorie" << category << "n'existe pas.";
    return QString();
}

QJsonObject PrefManager::getCategory(const QString &category) const
{
    if (m_userPrefs.contains(category)) {
        return m_userPrefs.value(category).toObject();
    }
    if (m_defaultPrefs.contains(category)) {
        m_defaultPrefs.value(category).toObject();
    }

    qWarning() << "La catégorie : "<< category << "n'existe pas";
    return QJsonObject();
}
