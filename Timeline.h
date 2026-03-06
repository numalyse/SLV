#ifndef TIMELINE_H
#define TIMELINE_H

#include "TimelineRuler.h"
#include "TimelineVideoTrack.h"

#include <QWidget>

class Timeline : public QWidget
{

public:
    explicit Timeline(QWidget* parent = nullptr);

public slots:
    void updateCursor(int64_t vlcTime);

protected:
    void paintEvent(QPaintEvent *event) override;
    //void mousePressEvent(QMouseEvent * event) override;
    //void mouseReleaseEvent(QMouseEvent * event) override;

private:
    TimelineRuler* m_ruler = nullptr;
    TimelineVideoTrack* m_videoTrack = nullptr;
    int m_cursorPosition = 250;
};



#endif // TIMELINE_H
