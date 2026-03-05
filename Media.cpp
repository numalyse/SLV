#include "Media.h"
#include "VlcInstance.h"
#include "VlcParseHelper.h"

#include <qurl.h>


Media::Media(const QString &filePath, QObject *parent) : QObject(parent), m_filePath(filePath)
{

    QFile f(m_filePath);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(m_filePath);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    m_vlcMedia = libvlc_media_new_location(SLV::VlcInstance::get(), urlBytes.constData());

    if (!m_vlcMedia){
        qDebug() << "Erreur lors de l'allocation du média";
        return;
    }

    m_fileInfo = new QFileInfo(filePath);
    m_name = m_fileInfo->baseName();
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
}

void Media::parse(){
    if(m_vlcMedia){
        m_parseEventManager = libvlc_media_event_manager(m_vlcMedia);
        libvlc_event_attach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
        libvlc_media_parse_with_options(m_vlcMedia, libvlc_media_parse_local, 0);
    }
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
                auto duration = libvlc_media_get_duration(parsedMedia);
                
                media->setDuration(duration);
                media->setFps(fps);
                media->setMeta(VlcParseHelper::getMetaParsedMedia(parsedMedia));
                
                emit media->fpsParsed(fps);
                emit media->durationParsed(duration);
            }
        }
        else  {
            qDebug() << "Le parsing a changé de statut, mais n'est pas terminé. Statut :" << parseStatus;
        }

    }
}