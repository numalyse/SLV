#ifndef TIMELINE_H
#define TIMELINE_H

#include "Shot.h"


#include "Timeline/TimelineView.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/ShotManager.h"
#include "Timeline/ABManager.h"

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
#include <QTimer>

class TimelineWidget : public QWidget
{
Q_OBJECT

public:
    explicit TimelineWidget(QVector<Shot>& projectShots, QWidget* parent = nullptr);
    QVector<Shot> getTimelineData();

public slots:
    void updateCursorPos(int64_t vlcTime);
    void updateCursorVisually(int sliderValue);
    void goToShot(int);
    void mergeWithPrevShotAction();
    void mergeWithNextShotAction();

    const QVector<ShotItem*>& shotItems() const { return m_shotManager->shotItems();};

signals:
    void updateShotDetailRequest(int shotCount, int shotId, Shot*);
    void timelineSetPosition(int64_t);
    void timelineSliderPositionRequested(int64_t);
    void enableTimeRelatedUI();
    void disableTimeRelatedUI();
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void splitShotAtCursor();
    void ABAction();
    void moveCursor(double cursorPosX);
    void itemLeftClick(QGraphicsItem*);
    void itemRightClick(QPoint, QGraphicsItem*);
    void updateShowMergeWithNextShot(bool);
    void updateShowMergeWithPreviousShot(bool);

private:
    void applyZoom(double zoomFactor, int mouseX);
    
    void showContextMenuForShot(const QPoint& globalPos, ShotItem *item);

    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;

    TimelineMath* m_mathManager = nullptr;
    ShotManager* m_shotManager = nullptr;
    ABManager* m_abManager = nullptr;

    ToolbarButton* m_splitShotBtn = nullptr;
    ToolbarButton* m_abLoopBtn = nullptr;
    ToolbarButton* m_mergeWithPrevShotBtn = nullptr;
    bool m_showMergeWithPrevShotBtn = false;
    ToolbarButton* m_mergeWithNextShotBtn = nullptr;
    bool m_showMergeWithNextShotBtn = false;

    QTimer* m_seekTimer = nullptr;
    int m_seekPendingTime = 50;
    bool m_isDraggingCursor = false;

    int64_t m_vlcTime{};

    double m_minPxBetweenTicks = 100.0;

    int m_sceneWidth = 10000;
    int m_sceneHeight = 150;

    int m_rulerHeight = 25;


};



#endif // TIMELINE_H
