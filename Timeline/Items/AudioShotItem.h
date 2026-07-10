#ifndef AUDIOSHOTITEM_H
#define AUDIOSHOTITEM_H

#include "ShotItem.h"

class AudioShotItem : public ShotItem
{
private:
    AudioShot m_shot;

public:
    explicit AudioShotItem(AudioShot shot, double width, QGraphicsItem* parent = nullptr);

    int type() const override { return SLV::TypeAudioShotItem; }

protected:
    double height() const override { return s_height; }
    double topMargin() const override { return s_topMargin; }

private:
    static constexpr double s_height = 40.0;
    static constexpr double s_topMargin = 93.0;
};

#endif // AUDIOSHOTITEM_H
