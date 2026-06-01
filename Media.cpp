#include "Media.h"
#include "VlcInstance.h"
#include "VlcParseHelper.h"
#include "PrefManager.h"

#include <QUrl>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>


Media::Media(const QString &filePath, QObject *parent, libvlc_instance_t *vlcInstance) : QObject(parent), m_filePath(filePath)
{
    // Créer m_fileInfo en premier pour éviter les null-ptr dereferences
    m_fileInfo = new QFileInfo(filePath);

    QFile f(m_filePath);
    qDebug() << "[Media::constructor] Vérification du fichier" << filePath << "- Existe?" << f.exists();

    QUrl url = QUrl::fromLocalFile(m_filePath);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();
    qDebug() << "[Media::constructor] URL convertie:" << url.toString();

    if(vlcInstance) m_vlcInstance = vlcInstance;
    else m_vlcInstance = SLV::VlcInstance::get();

    if (!m_vlcInstance) {
        qDebug() << "[Media::constructor] Erreur : instance VLC non disponible";
        return;
    }
    
    qDebug() << "[Media::constructor] VLC instance obtenue, création du média";

    m_vlcMedia = libvlc_media_new_location(m_vlcInstance, urlBytes.constData());

    if (!m_vlcMedia){
        qDebug() << "[Media::constructor] Erreur lors de l'allocation du média VLC";
        return;
    }

    qDebug() << "[Media::constructor] Média VLC créé avec succès";

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
                libvlc_media_track_t **tracks;
                unsigned int count = libvlc_media_tracks_get(parsedMedia, &tracks);
                bool hasVideo = false; bool hasAudio = false; bool hasText = false;
                MediaType type = MediaType::Unknown;
                for (unsigned int i = 0; i < count; ++i) {
                    MediaTrackInfo trackInfo = MediaTrackInfo();
                    trackInfo._description = tracks[i]->psz_description;
                    trackInfo._type = tracks[i]->i_type;
                    switch (tracks[i]->i_type) {
                    case libvlc_track_video:
                        hasVideo = true;
                        trackInfo._bitrate = tracks[i]->i_bitrate;
                        trackInfo._codec = tracks[i]->i_codec;
                        trackInfo._sarNum = tracks[i]->video->i_sar_num;
                        trackInfo._sarDen = tracks[i]->video->i_sar_den;
                        break;
                    case libvlc_track_audio:
                        hasAudio = true;
                        trackInfo._bitrate = tracks[i]->i_bitrate;
                        trackInfo._codec = tracks[i]->i_codec;
                        trackInfo._channels = tracks[i]->audio->i_channels;
                        trackInfo._rate = tracks[i]->audio->i_rate;
                        trackInfo._language = tracks[i]->psz_language;
                        break;
                    case libvlc_track_text:
                        trackInfo._encoding = tracks[i]->subtitle->psz_encoding;
                        trackInfo._language = tracks[i]->psz_language;
                        break;
                    case libvlc_track_unknown:
                        break;
                    }
                    media->addTrack(trackInfo);
                }
                if(hasVideo)
                    type = MediaType::Video;
                else if(hasAudio)
                    type = MediaType::Audio;
                //à voir pour image

                QMap<libvlc_meta_t, QString> metaData;
                for(int IMeta = libvlc_meta_Title; IMeta <= libvlc_meta_DiscTotal; ++IMeta){
                    libvlc_meta_t meta = static_cast<libvlc_meta_t>(IMeta);
                    metaData[meta] = libvlc_media_get_meta(parsedMedia, meta);
                }
                media->setMeta(metaData);

                libvlc_media_tracks_release(tracks, count);
                
                media->setDuration(duration);
                media->setFps(fps);
                media->setHeight(std::get<0>(resolution));
                media->setWidth(std::get<1>(resolution));

                media->setMeta(VlcParseHelper::getMetaParsedMedia(parsedMedia));

                emit media->fpsParsed(fps);
                emit media->resolutionParsed(resolution);
                emit media->durationParsed(duration);
                emit media->typeParsed(type);
            }
        }
        else  {
            qDebug() << "Le parsing a changé de statut, mais n'est pas terminé. Statut :" << parseStatus;
        }

    }
}

QString Media::metaToString(const libvlc_meta_t meta) const
{
    switch(meta){
    case libvlc_meta_Title:
        return PrefManager::instance().getText("media_meta_title");
    case libvlc_meta_Artist:
        return PrefManager::instance().getText("media_meta_artist");
    case libvlc_meta_Genre:
        return PrefManager::instance().getText("media_meta_genre");
    case libvlc_meta_Copyright:
        return PrefManager::instance().getText("media_meta_copyright");
    case libvlc_meta_Album:
        return PrefManager::instance().getText("media_meta_album");
    case libvlc_meta_TrackNumber:
        return PrefManager::instance().getText("media_meta_track_number");
    case libvlc_meta_Description:
        return PrefManager::instance().getText("media_meta_description");
    case libvlc_meta_Rating:
        return PrefManager::instance().getText("media_meta_rating");
    case libvlc_meta_Date:
        return PrefManager::instance().getText("media_meta_date");
    case libvlc_meta_Setting:
        return PrefManager::instance().getText("media_meta_setting");
    case libvlc_meta_URL:
        return PrefManager::instance().getText("media_meta_url");
    case libvlc_meta_Language:
        return PrefManager::instance().getText("media_meta_language");
    case libvlc_meta_NowPlaying:
        return PrefManager::instance().getText("media_meta_now_playing");
    case libvlc_meta_Publisher:
        return PrefManager::instance().getText("media_meta_publisher");
    case libvlc_meta_EncodedBy:
        return PrefManager::instance().getText("media_meta_encoded_by");
    case libvlc_meta_ArtworkURL:
        return PrefManager::instance().getText("media_meta_artwork_url");
    case libvlc_meta_TrackID:
        return PrefManager::instance().getText("media_meta_track_id");
    case libvlc_meta_TrackTotal:
        return PrefManager::instance().getText("media_meta_track_total");
    case libvlc_meta_Director:
        return PrefManager::instance().getText("media_meta_director");
    case libvlc_meta_Season:
        return PrefManager::instance().getText("media_meta_season");
    case libvlc_meta_Episode:
        return PrefManager::instance().getText("media_meta_episode");
    case libvlc_meta_ShowName:
        return PrefManager::instance().getText("media_meta_show_name");
    case libvlc_meta_Actors:
        return PrefManager::instance().getText("media_meta_actors");
    case libvlc_meta_AlbumArtist:
        return PrefManager::instance().getText("media_meta_album_artist");
    case libvlc_meta_DiscNumber:
        return PrefManager::instance().getText("media_meta_disc_number");
    case libvlc_meta_DiscTotal:
        return PrefManager::instance().getText("media_meta_disc_total");
    }
}
