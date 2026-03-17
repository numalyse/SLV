#ifndef SHOTMANAGER_H
#define SHOTMANAGER_H

#include "Timeline/TimelineMath.h"
#include "Timeline/Items/ShotItem.h"
#include "Timeline/TimelineView.h"

#include <QObject>
#include <QGraphicsScene>

class ShotManager : public QObject
{
Q_OBJECT

public:
    explicit ShotManager( QGraphicsScene* scene, TimelineView* view, TimelineMath* mathManager, QVector<Shot>& projectShots,  QObject* parent = nullptr);

    void updateCurrentShot(int64_t time);

    void splitShotAt(int64_t time);

    void updateShotItemsPosition();

    std::optional<double> getStartPosOf(int idShot);

signals:
    void updateShotDetailRequested(int shotCount, int shotId, Shot*);

private:

    QVector<ShotItem*> m_shotItems;
    ShotItem* m_currentShotItem = nullptr;

    // pointeurs non onwner, p_ pour les différencier
    QGraphicsScene* p_scene = nullptr;
    TimelineView* p_view = nullptr;
    TimelineMath* p_mathManager = nullptr;
};




#endif