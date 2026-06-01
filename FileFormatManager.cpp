#include "FileFormatManager.h"
#include "PrefManager.h"

#include <QFile>
#include <QJsonArray>

FileFormatManager::FileFormatManager()
{
    QString filePath = ":/formats";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_formatsJson = doc.object();
        file.close();
    } else {
        qDebug() << "Impossible d'ouvrir le fichier de formats " << filePath;
    }
}

const bool FileFormatManager::isFormatAccepted(const QString& format)
{
    const QStringList allowedFormats = getFormatsList("containers_video") + getFormatsList("containers_audio") + getFormatsList("images");
    return allowedFormats.contains(format);
}

const QString FileFormatManager::getOpenFileDialogFilters()
{
    PrefManager prefManager = PrefManager::instance();
    return prefManager.getText("file_multimedia") + " ("
        + FileFormatManager::instance().getFormats("containers_video")
        + FileFormatManager::instance().getFormats("containers_audio")
        + FileFormatManager::instance().getFormats("images")
        + ");;"
        + prefManager.getText("file_video") + " (" + FileFormatManager::instance().getFormats("containers_video") + ");;"
        + prefManager.getText("file_audio") + " (" +  FileFormatManager::instance().getFormats("containers_audio") + ");;"
        + prefManager.getText("file_image") + " (" +  FileFormatManager::instance().getFormats("images") + ')';
}

const QStringList FileFormatManager::getFormatsList(const QString& type)
{
    QStringList formatsList;
    for (const auto &v : m_formatsJson[type].toArray())
        formatsList << v.toString();
    return formatsList;
}

const QString FileFormatManager::getFormats(const QString& type)
{
    QString formats = "";
    for (const auto &v : m_formatsJson[type].toArray())
        formats += "*." + v.toString() + " ";
    return formats;
}
