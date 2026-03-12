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
    explicit TimelineWidget(QVector<Shot>& projectShots, QWidget* parent = nullptr);

public slots:
    void updateCursorPos(int64_t vlcTime);
    double timeToPosition(int64_t time);


signals:
    void updateShotDetailRequested(int shotId, Shot*);
    void timelineSetPosition(int64_t);
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void splitCurrentShotItem();
    void moveCursor(double cursorPosX);
    void itemClicked(QGraphicsItem*);



private:
    void applyZoom(double zoomFactor, int mouseX);
    int64_t timeAtCursor();
    void updateCurrentShot();
    void updateShotItems();

    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;
    QVector<ShotItem*> m_shotItems;
    ShotItem* m_currentShotItem = nullptr;

    ToolbarButton* m_splitShotBtn = nullptr;

    double m_fps{};
    int64_t m_duration{};
    int64_t m_vlcTime{};

    int m_sceneWidth = 2000;
    int m_sceneHeight = 150;
    double m_pixelsPerMs {};

    int m_rulerHeight = 25;
    double m_minPxBetweenTicks = 100.0;




};



#endif // TIMELINE_H
