
#ifndef VLCINSTANCE_H
#define VLCINSTANCE_H

#include <vlc/vlc.h>
#include <QDebug>
#include <cstdlib>
#include <cstdio>

namespace SLV
{
    


class VlcInstance
{

public:

    static libvlc_instance_t* get() {
        static VlcInstance _instance;
        if (!_instance.m_instance) {
            fprintf(stderr, "[VlcInstance::get] ATTENTION: m_instance est nullptr!\n");
            fflush(stderr);
        } else {
            fprintf(stderr, "[VlcInstance::get] Retour de l'instance VLC valide\n");
            fflush(stderr);
        }
        return _instance.m_instance;
    };

    // Empêcher la copie/move du singleton
    VlcInstance(const VlcInstance&) = delete;
    VlcInstance& operator=(const VlcInstance&) = delete;
    VlcInstance(VlcInstance&&) = delete;
    VlcInstance& operator=(VlcInstance&&) = delete;

private:
    VlcInstance() {
        fprintf(stderr, "[VlcInstance::constructor] Initialisation du singleton VLC\n");
        fflush(stderr);
        
#ifdef Q_OS_WIN
        const char* const vlc_args[] = {
            "--quiet",
            "--aout=directsound",
            "--no-video-title-show",
            "--no-input-fast-seek"
        };
        fprintf(stderr, "[VlcInstance::constructor] Configuration Windows détectée\n");
#elif defined(Q_OS_MAC)
        // Sur macOS, VLC est installé comme application VLC.app
        // Les plugins se trouvent dans le bundle VLC.app
        const char* vlcPluginPath = "/Applications/VLC.app/Contents/MacOS/plugins";
        if (!std::getenv("VLC_PLUGIN_PATH")) {
            setenv("VLC_PLUGIN_PATH", vlcPluginPath, 0);
            fprintf(stderr, "[VlcInstance::constructor] VLC_PLUGIN_PATH défini sur %s\n", vlcPluginPath);
        } else {
            fprintf(stderr, "[VlcInstance::constructor] VLC_PLUGIN_PATH déjà défini: %s\n", std::getenv("VLC_PLUGIN_PATH"));
        }

        const char* const vlc_args[] = {
            "--quiet",
            "--no-video-title-show",
            "--no-input-fast-seek",
            "--verbose=2",
            "--logfile=/tmp/slv-vlc.log"
        };
        fprintf(stderr, "[VlcInstance::constructor] Configuration macOS détectée avec environnement VLC_PLUGIN_PATH et logging\n");
#else
        const char* const vlc_args[] = {
            "--quiet",
            "--no-video-title-show",
            "--no-input-fast-seek"
        };
        fprintf(stderr, "[VlcInstance::constructor] Configuration Linux détectée\n");
#endif

        int argc = sizeof(vlc_args)/sizeof(vlc_args[0]);
        fprintf(stderr, "[VlcInstance::constructor] Appel à libvlc_new() avec %d arguments\n", argc);
        fflush(stderr);
        
        m_instance = libvlc_new(argc, vlc_args);
        
        if (!m_instance) {
            fprintf(stderr, "[VlcInstance::constructor] ERREUR CRITIQUE: libvlc_new() a échoué - VLC ne s'est pas initialisé\n");
            const char* errorMsg = libvlc_errmsg();
            if (errorMsg) {
                fprintf(stderr, "[VlcInstance::constructor] libvlc_errmsg: %s\n", errorMsg);
                libvlc_clearerr();
            }
            fprintf(stderr, "[VlcInstance::constructor] Vérifiez que VLC.app est installé dans /Applications/\n");
            fflush(stderr);
            return;
        }

        fprintf(stderr, "[VlcInstance::constructor] Instance VLC créée avec succès\n");
        fflush(stderr);
    };

    ~VlcInstance() { 
        if (m_instance) {
            libvlc_release(m_instance);
            m_instance = nullptr;
        }
    };

    libvlc_instance_t* m_instance  = nullptr;

};
} // namespace SLV


#endif