#pragma once

#include <vlc/vlc.h>
#include <QDebug>

class VlcInstance
{

public:

    static VlcInstance& instance() {
        static VlcInstance _instance;
        return _instance;
    };

    libvlc_instance_t* get(){ return m_vlcInstance ;};

private:
    VlcInstance() {
        const char* const vlc_args[] = {
            "--quiet",
            "--aout=directsound",
            "--no-video-title-show",
            "--no-input-fast-seek"
        };

        m_vlcInstance = libvlc_new(4, vlc_args);
        if (!m_vlcInstance) {
            qDebug() << "Erreur création VLC";
            return;
        }

    } ;
    libvlc_instance_t* m_vlcInstance  = nullptr;

};

