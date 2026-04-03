#include "Media.h"
#include "VlcInstance.h"
#include "VlcParseHelper.h"

#include <qurl.h>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>


Media::Media(const QString &filePath, QObject *parent, libvlc_instance_t *vlcInstance) : QObject(parent), m_filePath(filePath)
{

    QFile f(m_filePath);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(m_filePath);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    if(vlcInstance) m_vlcInstance = vlcInstance;
    else m_vlcInstance = SLV::VlcInstance::get();

    m_vlcMedia = libvlc_media_new_location(m_vlcInstance, urlBytes.constData());

    if (!m_vlcMedia){
        qDebug() << "Erreur lors de l'allocation du média";
        return;
    }

    m_fileInfo = new QFileInfo(filePath);
    m_name = m_fileInfo->baseName();
    setType(detectTypeFromFile(filePath));
    //qDebug() << m_name << " type détecté : " << m_type;
}

Media::~Media()
{
    if (m_parseEventManager){
        libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
        m_parseEventManager = nullptr;
    }
    if(m_vlcMedia){
        libvlc_media_release(m_vlcMedia);
        m_vlcMedia = nullptr;
    }
    delete m_fileInfo;
    m_fileInfo = nullptr;
}

void Media::parse(){
    if(m_vlcMedia){
        m_parseEventManager = libvlc_media_event_manager(m_vlcMedia);
        libvlc_event_attach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
        libvlc_media_parse_with_options(m_vlcMedia, libvlc_media_parse_local, 0);
    }
}

void Media::parseTracks(libvlc_media_player_t* player)
{
    if (!player) return;

    // qDebug() << "[MEDIA] parseTracks instance =" << this;

    // Audio
    libvlc_track_description_t *audio = libvlc_audio_get_track_description(player);
    m_audioTracks.clear();
    for (libvlc_track_description_t *t = audio; t != nullptr; t = t->p_next) {
        m_audioTracks.append(qMakePair(t->i_id, QString::fromUtf8(t->psz_name)));
    }
    libvlc_track_description_list_release(audio);

    // Sous-titres
    libvlc_track_description_t *subs = libvlc_video_get_spu_description(player);
    m_subtitlesTracks.clear();
    for (libvlc_track_description_t *t = subs; t != nullptr; t = t->p_next) {
        m_subtitlesTracks.append(qMakePair(t->i_id, QString::fromUtf8(t->psz_name)));
    }
    libvlc_track_description_list_release(subs);

    // qDebug() << "Audio tracks:";
    // for (auto &t : m_audioTracks)
    //     qDebug() << t.first << ":" << t.second;

    // qDebug() << "Subtitles tracks:";
    // for (auto &t : m_subtitlesTracks)
    //     qDebug() << t.first << ":" << t.second;    

    // qDebug() << "OK tracks parsed";

    emit tracksParsed();
    // qDebug() << "[MEDIA] SEND emit tracksParsed";
}

MediaType Media::detectTypeFromFile(const QString& path)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(path);

    QString name = type.name();

    if (name.startsWith("video/"))
        return MediaType::Video;
    if (name.startsWith("audio/"))
        return MediaType::Audio;
    if (name.startsWith("image/"))
        return MediaType::Image;

    return MediaType::Unknown;
}

/// @brief Ecoute les évènements vlc, lors du changement du temps envoie un signal.
/// Ecoute quand la lecture asychrone des métadonnées est terminée et envoie un signal.
/// @param event 
/// @param userData 
void Media::onVlcEvent(const libvlc_event_t *event, void *userData)
{
    Media* media = reinterpret_cast<Media*>(userData);

    if(event->type == libvlc_MediaParsedChanged ){

        int parseStatus = event->u.media_parsed_changed.new_status;

        if (parseStatus == 4){

            libvlc_media_t* parsedMedia = static_cast<libvlc_media_t*>(event->p_obj);

            if (parsedMedia) {
                auto fps = VlcParseHelper::getFpsParsedMedia(parsedMedia);
                std::tuple<int, int> resolution = VlcParseHelper::getResolutionParsedMedia(parsedMedia);
                auto duration = libvlc_media_get_duration(parsedMedia);
                
                media->setDuration(duration);
                media->setFps(fps);
                media->setHeight(std::get<0>(resolution));
                media->setWidth(std::get<1>(resolution));

                media->setMeta(VlcParseHelper::getMetaParsedMedia(parsedMedia));

                emit media->fpsParsed(fps);
                emit media->resolutionParsed(resolution);
                emit media->durationParsed(duration);
            }
        }
        else  {
            qDebug() << "Le parsing a changé de statut, mais n'est pas terminé. Statut :" << parseStatus;
        }

    }
}
