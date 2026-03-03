#include "Media.h"
#include "VlcInstance.h"
#include "VlcParseHelper.h"

#include <qurl.h>


Media::Media(const QString &filePath){
    m_filePath = filePath;
    QFile f(m_filePath);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(m_filePath);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    libvlc_media_t *media = libvlc_media_new_location(SLV::VlcInstance::get(), urlBytes.constData());

    if (!media){
        qDebug() << "Media ";
        return;
    }
 
    if(m_parseEventManager){ 
        libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
        m_parseEventManager = nullptr;
    }   

    m_parseEventManager = libvlc_media_event_manager(media);
    libvlc_event_attach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
    libvlc_media_parse_with_options(media, libvlc_media_parse_local, 0);

    QFileInfo *fileInfo = new QFileInfo(filePath);
    m_name = fileInfo->baseName();
    m_duration = libvlc_media_get_duration(media);

}

Media::~Media()
{
    if (m_parseEventManager){
        libvlc_event_detach(m_parseEventManager, libvlc_MediaParsedChanged, onVlcEvent, this);
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

                media->setDuration(libvlc_media_get_duration(parsedMedia));
                media->setFps(VlcParseHelper::getFpsParsedMedia(parsedMedia));
                media->setMeta(VlcParseHelper::getMetaParsedMedia(parsedMedia));

            }
        }
        else  {
            qDebug() << "Le parsing a changé de statut, mais n'est pas terminé. Statut :" << parseStatus;
        }

    }
}