#ifndef SHOTMANAGER_H
#define SHOTMANAGER_H

#include "Timeline/TimelineMath.h"
#include "Timeline/Items/ShotItem.h"
#include "Timeline/Items/AudioShotItem.h"
#include "Timeline/TimelineView.h"
#include "Timeline/ThumbnailWorker.h"
#include "VideoCaptureManager.h"
#include "Media.h"

#include "Shot.h"

#include <QObject>
#include <QGraphicsScene>
#include <QPair>
#include <QString>

class ShotManager : public QObject
{
Q_OBJECT

public:

    explicit ShotManager(QGraphicsScene *scene, TimelineView *view, TimelineMath *mathManager, ThumbnailWorker* thumbnailWorker, Media* media, QVector<Shot> &projectShots, QObject *parent);

    void updateCurrentShot(int64_t time);

    void splitShotAt(int64_t time);

    void mergeCurrentWithPrevShot(int64_t cursorTime);

    void mergeCurrentWithNextShot(int64_t cursorTime);

    void updateShotItemsPosition();

    std::optional<double> getStartXOf(int idShot);

    std::optional<int64_t> getStartTimeOf(int idShot);

    const QVector<ShotItem*>& shotItems() const {return m_shotItems;};
    size_t shotCount() const {return m_shotItems.size();};

    const QVector<Shot> shotItemsData() const;
    void extracted(int &shotHeight, const Shot &IShot, double &width);
    void setShotItemsData(const QVector<Shot> &shots);

    void createShotItemsFromCuts(const std::vector<int>& cuts);

    void initShotDetail();

    void toggleSelection(ShotItem* shotItem, AudioShotItem* audioShotItem, bool exclusive = false);
    
    void updateSelectedNumbers();
    void addSelected(QPair<ShotItem *, AudioShotItem *> &toBeAdded);
    void removeSelected(int elementIdToRemove, QPair<ShotItem *, AudioShotItem *> &toBeRemoved);
    void clearSelection();

    void extractShotsSelected(const QString& outputPath);

    int getNbShotsSelected() { return static_cast<int>(m_selectedShots.size()) ;}

    const int getCurrentShotId(){ return m_shotItems.indexOf(m_currentShotItem); }

signals:
    void updateShotDetailRequested(int shotCount, int requestId, Shot*);
    void showMergeWithPreviousShotAction(bool);
    void showMergeWithNextShotAction(bool);
    void shotsExtractionFinished(const QString& outputPath);
    void shotsExtractionFailed();
    void shotCountUpdated(int newShotCount);

private:
    void mergeCurrentInto(int ShotItemId);
    void mergeCurrentWithAdjacentShot(int indexOffset, int64_t cursorTime);

    /// @brief Request thumbnail update of a shot (will do nothing if media is not a video)
    void requestShotThumbnail(int shotId, const Shot& shot);

    QVector<ShotItem*> m_shotItems;
    QVector<AudioShotItem*> m_audioShotItems;
    QVector<QPair<ShotItem*, AudioShotItem*>> m_selectedShots;

    ShotItem* m_currentShotItem = nullptr;

    // pointeurs non onwner, p_ pour les différencier
    QGraphicsScene* p_scene = nullptr;
    TimelineView* p_view = nullptr;
    TimelineMath* p_mathManager = nullptr;
    Media* p_media = nullptr;

    ThumbnailWorker* p_thumbnailWorker = nullptr;
    int m_thumbnailWidth{100};
    int m_thumbnailHeight{75};

    VideoCaptureManager m_videoCaptureManager;

private slots:
    void updateThumbnail(int requestId, QImage image);

};




#endif
