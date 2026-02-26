#ifndef MEDIA_H
#define MEDIA_H

#include <vlc/vlc.h>
#include <QObject>
#include <QFileInfo>

enum class MediaType{
    Video,
    Image,
    Audio
};

class Media
{
public:
    Media(const QString &filePath);
    ~Media();
    QString fileName() const;
    QString filePath() const;
    QString fileExtension() const;
    qint64 fileSize() const;
    bool exists() const;


private:
    QString m_name;
    QString m_filePath;
    QFileInfo m_fileInfo;
    MediaType m_type;
    int m_duration; // ms ?
    QString m_metadata; // comment récupérer toutes les métadonnées ?
};

class MediaLoader
{
public:
    static Media loadFromFile(const QString& path);
};

#endif // MEDIA_H
