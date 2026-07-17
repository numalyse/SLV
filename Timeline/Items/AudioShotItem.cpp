#include "AudioShotItem.h"

AudioShotItem::AudioShotItem(AudioShot shot, double width, QGraphicsItem* parent)
: ShotItem(shot, width, parent){
    m_shot = shot;
}

