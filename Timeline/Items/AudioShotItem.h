#ifndef AUDIOSHOTITEM_H
#define AUDIOSHOTITEM_H

#include "ShotItem.h"

class AudioShotItem : public ShotItem
{
private:
    AudioShot m_shot;

public:
    explicit AudioShotItem(AudioShot shot, double width, double height = 40, double topMargin = 93, QGraphicsItem* parent = nullptr);

    int type() const override { return SLV::TypeAudioShotItem; }
};

#endif // AUDIOSHOTITEM_H
