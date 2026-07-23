#ifndef MEDIATRANSFORMHELPER_H
#define MEDIATRANSFORMHELPER_H

#include <vector>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <QCoreApplication>
#include <string>
#endif

/// @brief Manages arguments of vlc instance from transformations attributes
/// @param rotation : 0=0°, 1=90°, 2=180°, 3=270°
/// @param hflip : is media flipped horizontally
/// @param vflip : is media flipped vertically
/// @return Arguments needed for vlc instance
std::vector<const char*> getArgsFromTransform(const unsigned int rotation, const bool hflip, const bool vflip){

#ifdef Q_OS_WIN
    static thread_local std::string pluginArg = "--plugin-path=" + QCoreApplication::applicationDirPath().append("/plugins").toStdString();

    std::vector<const char*> transformArgs = {"--quiet",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--aout=directsound",
        "--no-osd",
        "--no-snapshot-preview",
        pluginArg.c_str(),
        "--verbose=2",
        "--video-filter=transform"
    };
    const std::vector<const char*> noTransformArgs = {"--quiet",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--aout=directsound",
        "--no-osd",
        "--no-snapshot-preview",
        pluginArg.c_str(),
        "--verbose=2"
    };
#else
    std::vector<const char*> transformArgs = {"--quiet",
        "--aout=directsound",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--no-osd",
        "--no-snapshot-preview",
        "--video-filter=transform"
    };
    const std::vector<const char*> noTransformArgs = {"--quiet",
        "--aout=directsound",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--no-osd",
        "--no-snapshot-preview"
    };
#endif

    switch(rotation){
    case 0:
        if(hflip && vflip) transformArgs.push_back("--transform-type=180");
        else if(hflip) transformArgs.push_back("--transform-type=hflip");
        else if(vflip) transformArgs.push_back("--transform-type=vflip");
        else return noTransformArgs;
        break;
    case 1:
        if(hflip && vflip) transformArgs.push_back("--transform-type=90");
        else if(hflip) transformArgs.push_back("--transform-type=antitranspose");
        else if(vflip) transformArgs.push_back("--transform-type=transpose");
        else transformArgs.push_back("--transform-type=270");
        break;
    case 2:
        if(hflip && vflip) return noTransformArgs;
        else if(hflip) transformArgs.push_back("--transform-type=vflip");
        else if(vflip) transformArgs.push_back("--transform-type=hflip");
        else transformArgs.push_back("--transform-type=180");
        break;
    case 3:
        if(hflip && vflip) transformArgs.push_back("--transform-type=270");
        else if(hflip) transformArgs.push_back("--transform-type=transpose");
        else if(vflip) transformArgs.push_back("--transform-type=antitranspose");
        else transformArgs.push_back("--transform-type=90");
        break;
    }
    return transformArgs;
};

#endif // MEDIATRANSFORMHELPER_H
