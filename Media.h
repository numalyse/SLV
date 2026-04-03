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
    explicit Media(const QString &filePath, QObject *parent = nullptr, libvlc_instance_t *vlcInstance = nullptr);

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
    int height() const { return m_height; }
    int width() const { return m_width; }
    libvlc_instance_t* vlcInstance() const { return m_vlcInstance; }
    libvlc_media_t* vlcMedia() const { return m_vlcMedia; }
    QMap<libvlc_meta_t, QString> metaData() const { return m_metaData; }

    void setType(MediaType type) { m_type = type; }
    void setDuration(int64_t duration) { m_duration = duration; }
    void setFps(double fps) { m_fps = fps; }
    void setHeight(int height) { m_height = height; }
    void setWidth(int width) { m_width = width; }
    void setMeta(QMap<libvlc_meta_t, QString> metaData) { m_metaData = metaData; }

    QList<QPair<int, QString>> audioTracks() const { return m_audioTracks; }
    QList<QPair<int, QString>> subtitlesTracks() const { return m_subtitlesTracks; }
    void parseTracks(libvlc_media_player_t* player);

signals:
    void fpsParsed(double);
    void resolutionParsed(std::tuple<int, int>);
    void durationParsed(int64_t);
    void sizeParsed(const QSize &size);
    void tracksParsed();
    void typeParsed(const MediaType);
    
private:
    QString m_name;
    QString m_filePath;
    QFileInfo* m_fileInfo = nullptr;
    MediaType m_type = MediaType::Unknown;
    int64_t m_duration {}; 
    double m_fps {};
    int m_height = 0;
    int m_width = 0;

    QMap<libvlc_meta_t, QString> m_metaData;

    libvlc_event_manager_t* m_parseEventManager = nullptr;
    libvlc_instance_t* m_vlcInstance = nullptr;
    libvlc_media_t * m_vlcMedia = nullptr;

    QList<QPair<int, QString>> m_audioTracks;
    QList<QPair<int, QString>> m_subtitlesTracks;

    static void onVlcEvent(const libvlc_event_t *event, void *userData);
};


#endif // MEDIA_H
