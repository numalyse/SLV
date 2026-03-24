#ifndef VLCPARSEHELPER
#define VLCPARSEHELPER

#include <vlc/vlc.h>
#include <QMap>

namespace VlcParseHelper
{

    /// @brief Helper pour parcourir les métadonnées et récuperer les non vides.
    /// @param parsedMedia event->type == libvlc_MediaParsedChanged
    /// @return Map avec clé enum libvlc et valeur QString son contenu
    inline QMap<libvlc_meta_t, QString> getMetaParsedMedia( libvlc_media_t* parsedMedia ) {
        QMap<libvlc_meta_t, QString> metaMap;
        for (int IMeta = libvlc_meta_Title; IMeta != libvlc_meta_DiscTotal ; IMeta++) {
            
            libvlc_meta_t metaType = static_cast<libvlc_meta_t>(IMeta);
            char* metaCStr = libvlc_media_get_meta(parsedMedia, metaType);
            
            if (metaCStr) {
                QString metaText = QString::fromUtf8(metaCStr);
                metaMap[metaType] = metaText;
                libvlc_free(metaCStr);
            }
        }
        return metaMap;
        
    }

    /// @brief Helper pour parcourir les streams d'un média et récuperer ses fps
    /// @param parsedMedia 
    /// @return double qui représente les fps moyens de la vidéo, -1 si le parsed média est null, 0.0 si pas de fps retrouvé, 
    inline double getFpsParsedMedia( libvlc_media_t* parsedMedia ){

        if (!parsedMedia) return -1;

        libvlc_media_track_t** tracks = nullptr;
        unsigned int tracksCount = libvlc_media_tracks_get(parsedMedia, &tracks);
        double mediaFps = 0.0;

        for (unsigned int ITrack = 0; ITrack < tracksCount; ++ITrack) {
            
            if (tracks[ITrack]->i_type == libvlc_track_video) {

                unsigned int num = tracks[ITrack]->video->i_frame_rate_num;
                unsigned int den = tracks[ITrack]->video->i_frame_rate_den;
                
                if (den > 0) {
                    mediaFps = static_cast<float>(num) / static_cast<float>(den);
                }
            
                break; 
            }
        }

        if (tracksCount > 0) {
            libvlc_media_tracks_release(tracks, tracksCount);
        }

        return mediaFps;
    }

    /// @brief Helper pour parcourir les streams d'un média et récuperer sa résolution
    /// @param parsedMedia
    /// @return std::tuple<int, int> qui correspond respectivement à la hauteur et à la largeur du média, et (-1, -1) si le parsed media est null
    inline std::tuple<int, int> getResolutionParsedMedia( libvlc_media_t* parsedMedia ){

        if (!parsedMedia) return std::tuple<int, int>(-1, -1);

        libvlc_media_track_t** tracks = nullptr;
        unsigned int tracksCount = libvlc_media_tracks_get(parsedMedia, &tracks);
        unsigned int height = 0;
        unsigned int width = 0;

        for (unsigned int ITrack = 0; ITrack < tracksCount; ++ITrack) {

            if (tracks[ITrack]->i_type == libvlc_track_video) {

                height = tracks[ITrack]->video->i_height;
                width = tracks[ITrack]->video->i_width;

                break;
            }
        }

        if (tracksCount > 0) {
            libvlc_media_tracks_release(tracks, tracksCount);
        }

        return std::tuple<int, int>(height, width);
    }
} // namespace VlcParseHelper


#endif // VLCPARSEHELPER
