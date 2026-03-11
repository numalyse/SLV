#ifndef MEDIA_H
#define MEDIA_H

#include <vlc/vlc.h>
#include <QObject>
#include <QFileInfo>

enum class MediaType{
    Video,
    Image,
    Audio,
    Unknown
};

class Media : public QObject
{
Q_OBJECT

public:
    explicit Media(const QString &filePath, QObject *parent = nullptr);

    ~Media();

    void parse();

    QString fileName() const { return m_name; }
    QString filePath() const { return m_filePath; }
    QString fileExtension() const { return m_fileInfo->suffix(); }
    qint64 fileSize() const { return m_fileInfo->size(); }
    bool exists() const { return m_fileInfo->exists(); }
    MediaType type() const { return m_type; }
    int64_t duration() const { return m_duration; }
    double fps() const { return m_fps; }
    libvlc_media_t* vlcMedia() const { return m_vlcMedia; }

    void setType(MediaType type) { m_type = type; }
    void setDuration(int64_t duration) { m_duration = duration; }
    void setFps(double fps) { m_fps = fps; }
    void setMeta(QMap<libvlc_meta_t, QString> metaData) { m_metaData = metaData; }

signals:
    void fpsParsed(double);
    void durationParsed(int64_t);
    
private:
    QString m_name;
    QString m_filePath;
    QFileInfo* m_fileInfo = nullptr;
    MediaType m_type = MediaType::Unknown;
    int64_t m_duration {}; 
    double m_fps {};

    QMap<libvlc_meta_t, QString> m_metaData;

    libvlc_event_manager_t* m_parseEventManager = nullptr;
    libvlc_media_t * m_vlcMedia = nullptr;

    static void onVlcEvent(const libvlc_event_t *event, void *userData);
};


#endif // MEDIA_H
