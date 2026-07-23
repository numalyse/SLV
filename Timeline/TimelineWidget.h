#ifndef TIMELINE_H
#define TIMELINE_H

#include "Shot.h"
#include "Media.h"

#include "Timeline/TimelineView.h"
#include "Timeline/TimelineMath.h"
#include "Timeline/ShotManager.h"
#include "Timeline/ABManager.h"
#include "Timeline/SegmentationThread.h"
#include "Timeline/TransitionManager.h"
#include "Timeline/ThumbnailWorker.h"
#include "Timeline/AnnotationItemManager.h"

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
#include <QProcess>
#include <QPointer>
#include <QLabel>

class TimelineWidget : public QWidget
{
Q_OBJECT

public:

    explicit TimelineWidget(ThumbnailWorker* thumbnailWorker, Media* projectMedia, QVector<Shot> &projectShots, QWidget *parent, const int timelineWidth = 0);
    ~TimelineWidget();
    QVector<Shot> getTimelineData();
    void setTimelineData(QVector<Shot> shots);
    AnnotationItemManager* annotItemManager() { return m_annotItemManager; };

public slots:
    void updateCursorPos(int64_t vlcTime);
    void updateCursorVisually(int sliderValue);
    void goToShot(int);
    void mergeWithPrevShotAction();
    void mergeWithNextShotAction();
    void computeMediaAmplitudes(const QString& mediaPath);
    void initAudioVisualizer();
    void initShotDetail();
    bool event(QEvent *event) override;
    void updateShotCount(int shotCount);
    void setPlayerPlaying(bool playing) { m_isPlayerPlaying = playing; };

    const QVector<ShotItem*>& shotItems() const { return m_shotManager->shotItems();};

    ToolbarButton* abLoopBtn()            const { return m_abLoopBtn; }
    ToolbarButton* autoSegmentationBtn()  const { return m_autoSegmentationBtn; }
    ToolbarButton* splitShotBtn()         const { return m_splitShotBtn; }
    ToolbarButton* mergeWithPrevShotBtn() const { return m_mergeWithPrevShotBtn; }
    ToolbarButton* toPrevShotBtn()        const { return m_toPrevShotBtn; }
    ToolbarButton* shotInfoBtn()          const { return m_shotInfo; }
    ToolbarButton* toNextShotBtn()        const { return m_toNextShotBtn; }
    ToolbarButton* mergeWithNextShotBtn() const { return m_mergeWithNextShotBtn; }
    ToolbarButton* exportBtn()            const { return m_exportBtn; }

signals:
    void updateShotDetailRequest(int shotCount, int requestId, Shot*);
    void timelineSetPosition(int64_t);
    void timelineSliderPositionRequested(int64_t);
    void enableTimeRelatedUI();
    void disableTimeRelatedUI();
    void saveNeeded();
    void playerPauseRequested();
    void playerPlayRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void splitShotAtCursor();
    void ABAction();
    void moveCursor(double cursorPosX);
    void itemRightClick(QPoint, QGraphicsItem*);
    void updateShowMergeWithNextShot(bool);
    void updateShowMergeWithPreviousShot(bool);
    void updateTimelineGeometry();
    void autoSegmentation();
    void exportDone(const QString& text, const QString& outputPath);
    void fitSceneToViewport();

private:
    void applyZoom(double zoomFactor, int mouseX);

    void showContextMenuForShot(const QPoint& globalPos, ShotItem *item);

    Media* p_media = nullptr; // non owner
    QGraphicsScene* m_scene = nullptr;
    TimelineView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;
    AudioVisualizerItem* m_audioVisualizer = nullptr;

    TimelineMath* m_mathManager = nullptr;
    ShotManager* m_shotManager = nullptr;
    ABManager* m_abManager = nullptr;
    AnnotationItemManager* m_annotItemManager = nullptr;

    QPointer<SegmentationThread> m_segmThread;
    ToolbarButton* m_autoSegmentationBtn = nullptr;
    ToolbarButton* m_splitShotBtn = nullptr;
    ToolbarButton* m_abLoopBtn = nullptr;
    ToolbarButton* m_mergeWithPrevShotBtn = nullptr;
    bool m_showMergeWithPrevShotBtn = false;
    ToolbarButton* m_mergeWithNextShotBtn = nullptr;
    bool m_showMergeWithNextShotBtn = false;
    ToolbarButton* m_exportBtn = nullptr;
    ToolbarButton* m_shotInfo = nullptr;
    ToolbarButton* m_toPrevShotBtn = nullptr;
    ToolbarButton* m_toNextShotBtn = nullptr;

    ToolbarButton* m_openAnnot = nullptr;
    ToolbarButton* m_addAnnotBtn = nullptr;

    QLabel* m_shotCountLabel = nullptr;

    QTimer* m_seekTimer = nullptr;
    int m_seekPendingTime = 50;
    bool m_isDraggingCursor = false;
    bool m_wasPlayingBeforeDrag = false;

    int64_t m_vlcTime{};

    double m_minPxBetweenTicks = 100.0;

    int m_sceneWidth = 10000;
    int m_sceneHeight = 150;

    int m_rulerHeight = 25;

    QProcess *m_audioComputeProcess = nullptr;
    QByteArray m_audioBuffer;
    QVector<double> m_amplitudeList;

    // update via player signals 
    bool m_isPlayerPlaying = false;

};



#endif // TIMELINE_H
