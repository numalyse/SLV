#ifndef AUDIOSHOTITEM_H
#define AUDIOSHOTITEM_H

#include "ShotItem.h"

class AudioShotItem : public ShotItem
{
private:
    AudioShot m_shot;
public:
    explicit AudioShotItem(AudioShot shot, double width, double height = 40, double topMargin = 73, QGraphicsItem* parent = nullptr);
};

#endif // AUDIOSHOTITEM_H
