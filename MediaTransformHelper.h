#ifndef MEDIATRANSFORMHELPER_H
#define MEDIATRANSFORMHELPER_H

#include <vector>

/// @brief Manages arguments of vlc instance from transformations attributes
/// @param rotation : 0=0°, 1=90°, 2=180°, 3=270°
/// @param hflip : is media flipped horizontally
/// @param vflip : is media flipped vertically
/// @return Arguments needed for vlc instance
std::vector<const char*> getArgsFromTransform(const unsigned int rotation, const bool hflip, const bool vflip){
    std::vector<const char*> transformArgs = {"--quiet",
        "--aout=directsound",
        "--no-video-title-show",
        "--no-input-fast-seek",
        "--video-filter=transform"
    };
    switch(rotation){
    case 0:
        if(hflip && vflip) transformArgs.push_back("--transform-type=180");
        else if(hflip) transformArgs.push_back("--transform-type=hflip");
        else if(vflip) transformArgs.push_back("--transform-type=vflip");
        else return {"--quiet",
                "--aout=directsound",
                "--no-video-title-show",
                "--no-input-fast-seek"
            };
        break;
    case 1:
        if(hflip && vflip) transformArgs.push_back("--transform-type=90");
        else if(hflip) transformArgs.push_back("--transform-type=antitranspose");
        else if(vflip) transformArgs.push_back("--transform-type=transpose");
        else transformArgs.push_back("--transform-type=270");
        break;
    case 2:
        if(hflip && vflip) return {"--quiet",
                "--aout=directsound",
                "--no-video-title-show",
                "--no-input-fast-seek"
            };
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
