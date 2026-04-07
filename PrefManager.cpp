#include "PrefManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QDirIterator>


void PrefManager::loadLanguage(const QString& langCode) 
{

    QStringList availableLangs = getAvailableLangs();

    // si le langcode est bien dans les langues supportées, utilisation de celui ci sinon fallback sur en
    QString filePath = (availableLangs.contains(langCode)) ? ":/lang/" + langCode + ".json" : ":/lang/en.json";
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
    syncUserPrefs();
}


QString PrefManager::getPref(const QString &category, const QString &key) const
{
    QJsonObject catObj = getCategory(category);

    if (catObj.contains(key)) {
        return catObj.value(key).toString();
    }

    qWarning() << "[PrefManager] La clé" << key << "dans la catégorie" << category << "n'existe pas.";
    return QString();
}

QString PrefManager::getPref(const QString &category, const QString &subCategory, const QString &key) const
{
    QJsonObject subCatObj = getSubCategory(category, subCategory);

    if (subCatObj.contains(key)) {
        return subCatObj.value(key).toString();
    }

    qWarning() << "[PrefManager]  La clé" << key << "dans la catégorie" << category << "->" << subCategory << "n'existe pas.";
    return QString();
}


QJsonObject PrefManager::getCategory(const QString &category) const
{
    QJsonObject result; 

    if (m_defaultPrefs.contains(category)) {
        result = m_defaultPrefs.value(category).toObject();
    }

    if (m_userPrefs.contains(category)) { // ajoute les pref de l'utilsateur par dessus, si le fichier en contient que quelques uns ca sera pris en compte
        QJsonObject userCatObj = m_userPrefs.value(category).toObject();

        for (auto it = userCatObj.begin(); it != userCatObj.end(); ++it) {
            if (result.contains(it.key())) {
                result.insert(it.key(), it.value());
            } else {
                qWarning() << "[PrefManager] Clé inconnue ignorée :" << it.key() << "dans la catégorie" << category;
            }
        }
    }

    if (result.isEmpty()) {
        qWarning() << "[PrefManager] La catégorie : "<< category << "n'existe pas";
    }

    return result;
}


QJsonObject PrefManager::getSubCategory(const QString &category, const QString &subCategory) const
{
    QJsonObject result; 

    if (m_defaultPrefs.contains(category)) {
        QJsonObject defaultCatObj = m_defaultPrefs.value(category).toObject();
        if (defaultCatObj.contains(subCategory)) {
            result = defaultCatObj.value(subCategory).toObject();
        }
    }

    if (m_userPrefs.contains(category)) {
        QJsonObject userCatObj = m_userPrefs.value(category).toObject();

        if (userCatObj.contains(subCategory)) {
            QJsonObject userSubCatObj = userCatObj.value(subCategory).toObject();
            
            for (auto it = userSubCatObj.begin(); it != userSubCatObj.end(); ++it) {
                if (result.contains(it.key())) {
                    result.insert(it.key(), it.value());
                } else {
                    qWarning() << "Clé inconnue ignorée :" << it.key() << "dans" << category << "->" << subCategory;
                }
            }
        }
    }

    if (result.isEmpty()) {
        qWarning() << "La sous catégorie : "<< category << "->" << subCategory << "n'existe pas (ou est vide)";
    }

    return result;
}


bool PrefManager::setPref(const QString& category, const QString& key, const QString& value)
{
    QJsonObject categoryObject;
    if (m_userPrefs.contains(category) && m_userPrefs[category].isObject()) {
        categoryObject = m_userPrefs[category].toObject();
    }

    categoryObject[key] = value;
    m_userPrefs[category] = categoryObject;

    QString filePath = QDir(QDir::homePath()).filePath("SLV_Content/pref.json");
    QFile file(filePath);

    if (!file.exists() && !createPreferenceFile(filePath)) {
        qCritical() << "[PrefManager] Impossible de créer le fichier de préférences.";
        return false; 
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument newDoc(m_userPrefs);
        file.write(newDoc.toJson(QJsonDocument::Indented));
        file.close();
        
        return true;
    } 
    
    qDebug() << "[PrefManager] Erreur : Impossible d'écrire dans le fichier" << filePath;
    return false;
}

bool PrefManager::writeUserJson(){
    QString filePath = QDir(QDir::homePath()).filePath("SLV_Content/pref.json");
    QFile file(filePath);

    if (!file.exists() && !createPreferenceFile(filePath)) {
        qCritical() << "[PrefManager] Impossible de créer le fichier de préférences.";
        return false; 
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument newDoc(m_userPrefs);
        file.write(newDoc.toJson(QJsonDocument::Indented));
        file.close();
        
        return true;
    } 
    
    qDebug() << "[PrefManager] Erreur : Impossible d'écrire dans le fichier" << filePath;
    return false;

} 

void PrefManager::syncUserPrefs()
{
    bool modified = false;

    // adds any missing keys from default pref to user pref 
    modified = mergeMissingKeys(m_defaultPrefs, m_userPrefs);

    // checks if user paths are set and sets them if not
    QJsonObject pathCategory = m_userPrefs.value("Paths").toObject();
    QString defaultPath = pathCategory.value("default").toString();
    bool pathModified = false;

    if (defaultPath.isEmpty()){
        defaultPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        pathCategory.insert("default", defaultPath);
        pathModified = true;
    }
    if(pathCategory.value("lp_project").toString().isEmpty()){
        pathCategory.insert("lp_project", defaultPath);
        pathModified = true;
    }
    if(pathCategory.value("lp_open_media").toString().isEmpty()){
        pathCategory.insert("lp_open_media", defaultPath);
        pathModified = true;
    }
    if(pathCategory.value("lp_export").toString().isEmpty()){
        pathCategory.insert("lp_export", defaultPath);
        pathModified = true;
    }
    if(pathCategory.value("lp_extract_sequence").toString().isEmpty()){
        pathCategory.insert("lp_extract_sequence", defaultPath);
        pathModified = true;
    }
    if(pathCategory.value("lp_capture").toString().isEmpty()){
        pathCategory.insert("lp_capture", defaultPath);
        pathModified = true;
    }
    
    if(pathModified){
        m_userPrefs.insert("Paths", pathCategory);
        modified = true;
    }

    // if m_userPrefs was modified rewrite the json with all its data
    if (modified){
        writeUserJson();
    } 
}

bool PrefManager::mergeMissingKeys(const QJsonObject& defaultObj, QJsonObject& userObj)
{
    bool modified = false;

    for (auto it = defaultObj.begin(); it != defaultObj.end(); ++it) {
        const QString& key = it.key();

        if (!userObj.contains(key)) {
            userObj.insert(key, it.value());
            modified = true;
        } 
        else if (it.value().isObject() && userObj.value(key).isObject()) {
            QJsonObject subUserObj = userObj.value(key).toObject();
            if ( mergeMissingKeys(it.value().toObject(), subUserObj) ) {
                userObj.insert(key, subUserObj);
                modified = true;
            }
        }
    }

    return modified;
}

QStringList PrefManager::getAvailableLangs(){
    QStringList langs;

    QDirIterator it(":/lang", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QFile f(it.next());
        QFileInfo fileInfo(f);
        langs.append(fileInfo.baseName());
    }

    return langs;
}