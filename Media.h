#ifndef MEDIA_H
#define MEDIA_H

#include <vlc/vlc.h>
#include <QObject>
#include <QFileInfo>

enum class MediaType{
    Video,
    Image,
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
    QString m_filePath;
    QFileInfo m_fileInfo;
};

#endif // MEDIA_H
