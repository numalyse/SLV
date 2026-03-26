#ifndef SHOTMANAGER_H
#define SHOTMANAGER_H

#include "Timeline/TimelineMath.h"
#include "Timeline/Items/ShotItem.h"
#include "Timeline/TimelineView.h"
#include "Timeline/ThumbnailWorker.h"

#include "Shot.h"

#include <QObject>
#include <QGraphicsScene>

class ShotManager : public QObject
{
Q_OBJECT

public:

    explicit ShotManager(QGraphicsScene *scene, TimelineView *view, TimelineMath *mathManager, const QString &projectMediaPath, QVector<Shot> &projectShots, QObject *parent);

    void updateCurrentShot(int64_t time);

    void splitShotAt(int64_t time);

    void mergeCurrentWithPrevShot(int64_t cursorTime);

    void mergeCurrentWithNextShot(int64_t cursorTime);

    void updateShotItemsPosition();

    std::optional<double> getStartXOf(int idShot);

    std::optional<int64_t> getStartTimeOf(int idShot);

    const QVector<ShotItem*>& shotItems() const {return m_shotItems;};

    const QVector<Shot> shotItemsData() const;
    void setShotItemsData(const QVector<Shot>& shots);

    void createShotItemsFromCuts(const std::vector<int>& cuts);

signals:
    void updateShotDetailRequested(int shotCount, int shotId, Shot*);
    void showMergeWithPreviousShotAction(bool);
    void showMergeWithNextShotAction(bool);

private:
    void mergeCurrentInto(int ShotItemId);

    QVector<ShotItem*> m_shotItems;
    const QString m_mediaPath;
    ShotItem* m_currentShotItem = nullptr;

    // pointeurs non onwner, p_ pour les différencier
    QGraphicsScene* p_scene = nullptr;
    TimelineView* p_view = nullptr;
    TimelineMath* p_mathManager = nullptr;

    ThumbnailWorker* m_thumbnailWorker = nullptr;

private slots:
    void updateThumbnail(int shotId, QImage image);

};




#endif