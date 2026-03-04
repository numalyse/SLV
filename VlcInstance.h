#pragma once

#include <vlc/vlc.h>
#include <QDebug>

namespace SLV
{
    


class VlcInstance
{

public:

    static libvlc_instance_t* get() {
        static VlcInstance _instance;
        return _instance.m_instance;
    };

private:
    VlcInstance() {
        const char* const vlc_args[] = {
            "--quiet",
            "--aout=directsound",
            "--no-video-title-show",
            "--no-input-fast-seek"
        };

        m_instance = libvlc_new(4, vlc_args);
        if (!m_instance) {
            qDebug() << "Erreur création VLC";
            return;
        }

    };

    ~VlcInstance() { libvlc_release(m_instance); };

    libvlc_instance_t* m_instance  = nullptr;

};
} // namespace SLV



