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

    QString fileName() const { return m_name; }
    QString filePath() const { return m_filePath; }
    QString fileExtension() const { return m_fileInfo.suffix(); }
    qint64 fileSize() const { return m_fileInfo.size(); }
    bool exists() const { return m_fileInfo.exists(); }
    MediaType type() const { return m_type; }
    int64_t duration() const { return m_duration; }
    double fps() const { return m_fps; }

    void setType(MediaType type) { m_type = type; }
    void setDuration(int64_t duration) { m_duration = duration; }
    void setFps(double fps) { m_fps = fps; }
    void setMeta(QMap<libvlc_meta_t, QString> metaData) { m_metaData = metaData; }

private:
    //static libvlc_instance_t* vlcInstance; // une instance pour tous les medias

    QString m_name;
    QString m_filePath;
    QFileInfo m_fileInfo;
    MediaType m_type;
    int64_t m_duration; 
    double m_fps;

    QMap<libvlc_meta_t, QString> m_metaData;
    libvlc_event_manager_t* m_parseEventManager = nullptr;

    static void onVlcEvent(const libvlc_event_t *event, void *userData);
};

class MediaLoader
{
public:
    static Media loadFromFile(const QString& path);
};

#endif // MEDIA_H
