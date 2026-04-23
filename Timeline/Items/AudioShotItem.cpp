#include "AudioShotItem.h"

AudioShotItem::AudioShotItem(AudioShot shot, double width, double height, double topMargin , QGraphicsItem* parent)
: ShotItem(shot, width, height, topMargin, parent){
    m_shot = shot;
}

