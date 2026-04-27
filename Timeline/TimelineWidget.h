#ifndef TIMELINE_H
#define TIMELINE_H

#include "Shot.h"
#include "Media.h"

#include "Timeline/TimelineView.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/ShotManager.h"
#include "Timeline/ABManager.h"
#include "Timeline/SegmentationThread.h"

#include "Timeline/Items/ABMarkerItem.h"
#include "Timeline/Items/RulerItem.h"
#include "Timeline/Items/CursorItem.h"
#include "Timeline/Items/ShotItem.h"
#include "Timeline/Items/AudioVisualizerItem.h"

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

    explicit TimelineWidget(double fps, int64_t duration, Media &projectMediaPath, QVector<Shot> &projectShots, QWidget *parent);
    QVector<Shot> getTimelineData();
    void setTimelineData(QVector<Shot> shots);

public slots:
    void updateCursorPos(int64_t vlcTime);
    void updateCursorVisually(int sliderValue);
    void goToShot(int);
    void mergeWithPrevShotAction();
    void mergeWithNextShotAction();
    void computeMediaAmplitudes(const QString& mediaPath);
    void initAudioVisualizer();

    const QVector<ShotItem*>& shotItems() const { return m_shotManager->shotItems();};

signals:
    void updateShotDetailRequest(int shotCount, int requestId, Shot*);
    void timelineSetPosition(int64_t);
    void timelineSliderPositionRequested(int64_t);
    void enableTimeRelatedUI();
    void disableTimeRelatedUI();
    void saveNeeded();
    
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
    void autoSegmentation();
    
private:
    void applyZoom(double zoomFactor, int mouseX);
    
    void showContextMenuForShot(const QPoint& globalPos, ShotItem *item);

    Media* m_media = nullptr;
    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;
    AudioVisualizerItem* m_audioVisualizer = nullptr;

    TimelineMath* m_mathManager = nullptr;
    ShotManager* m_shotManager = nullptr;
    ABManager* m_abManager = nullptr;

    ToolbarButton* m_autoSegmentationBtn = nullptr;
    ToolbarButton* m_splitShotBtn = nullptr;
    ToolbarButton* m_abLoopBtn = nullptr;
    ToolbarButton* m_mergeWithPrevShotBtn = nullptr;
    bool m_showMergeWithPrevShotBtn = false;
    ToolbarButton* m_mergeWithNextShotBtn = nullptr;
    bool m_showMergeWithNextShotBtn = false;
    ToolbarButton* m_exportBtn = nullptr;

    QTimer* m_seekTimer = nullptr;
    int m_seekPendingTime = 50;
    bool m_isDraggingCursor = false;

    int64_t m_vlcTime{};

    double m_minPxBetweenTicks = 100.0;

    int m_sceneWidth = 10000;
    int m_sceneHeight = 150;

    int m_rulerHeight = 25;

    QByteArray m_audioBuffer;
    QVector<double> m_amplitudeList;

};



#endif // TIMELINE_H
