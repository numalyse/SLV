#ifndef TIMELINE_H
#define TIMELINE_H

#include "Shot.h"

#include "RulerItem.h"
#include "CursorItem.h"
#include "ShotItem.h"
#include "TimelineView.h"

#include "ToolbarButtons/ToolbarButton.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QVector>


class TimelineWidget : public QWidget
{
Q_OBJECT

public:
    explicit TimelineWidget(QWidget* parent = nullptr);

public slots:
    void updateCursorPos(int64_t vlcTime);
    void applyZoom(double zoomFactor);

signals:
    void updateShotDetailRequested(Shot*);
    void timelineSetPosition(int64_t);
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void splitCurrentShotItem();
    void moveCursor(double cursorPosX);

private:
    int64_t timeAtCursor();
    void updateCurrentShot();

    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    ToolbarButton* m_splitShotBtn = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;

    double m_fps{};
    int64_t m_duration{};

    int m_sceneWidth = 2000;
    int m_sceneHeight = 150;
    double m_pixelsPerMs {};

    int m_rulerHeight = 25;
    double m_minPxBetweenTicks = 100.0;
    int64_t m_vlcTime{};

    QVector<ShotItem*> m_shotItems;
    ShotItem* m_currentShotItem = nullptr;
};



#endif // TIMELINE_H
