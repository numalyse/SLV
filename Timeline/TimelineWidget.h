#ifndef TIMELINE_H
#define TIMELINE_H

#include "Shot.h"


#include "Timeline/TimelineView.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/ShotManager.h"

#include "Timeline/Items/ABMarkerItem.h"
#include "Timeline/Items/RulerItem.h"
#include "Timeline/Items/CursorItem.h"
#include "Timeline/Items/ShotItem.h"

#include "ToolbarButtons/ToolbarButton.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QVector>
#include <QPoint>

struct ABLoopData{
    int64_t aTime;
    double aXPos;
    int64_t bTime;
    double bXPos;
};

class TimelineWidget : public QWidget
{
Q_OBJECT

public:
    explicit TimelineWidget(QVector<Shot>& projectShots, QWidget* parent = nullptr);

public slots:
    void updateCursorPos(int64_t vlcTime);

    void goToShot(int);

signals:
    void updateShotDetailRequest(int shotCount, int shotId, Shot*);
    void timelineSetPosition(int64_t);
    void enableSliderRequested();
    void disableSliderRequested();
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void splitShotAtCursor();
    void moveCursor(double cursorPosX);
    void itemLeftClick(QGraphicsItem*);
    void itemRightClick(QPoint, QGraphicsItem*);

private:
    void applyZoom(double zoomFactor, int mouseX);
    
    void showContextMenuForShot(const QPoint& globalPos, ShotItem *item);

    void ABAction();
    std::optional<ABLoopData> getABLoopData();
    void updateMarkerPos();

    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;

    QVector<ABMarkerItem*> m_abMarkersItems;

    ToolbarButton* m_splitShotBtn = nullptr;
    ToolbarButton* m_abLoopBtn = nullptr;

    TimelineMath* m_mathManager = nullptr;
    ShotManager* m_shotManager = nullptr;

    int64_t m_vlcTime{};

    int m_sceneWidth = 10000;
    int m_sceneHeight = 150;
    //double m_pixelsPerMs {};

    int m_rulerHeight = 25;
    double m_minPxBetweenTicks = 100.0;




};



#endif // TIMELINE_H
