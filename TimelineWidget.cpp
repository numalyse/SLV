#include "TimelineWidget.h"

#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"
#include "TextManager.h"

#include "RulerItem.h"
#include "CursorItem.h"

#include "ItemTypes.h"

#include "Shot.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QMenu>
#include <QAction>

#include <algorithm> 

/// @brief Créer une timeline avec les plan du projet
/// @param projectShots 
/// @param parent 
TimelineWidget::TimelineWidget(QVector<Shot>& projectShots, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 

    m_fps = ProjectManager::instance().projet()->media->fps();
    m_duration = ProjectManager::instance().projet()->media->duration();
    if(m_fps == 0.0 || m_duration == 0){
        qDebug() << "Creation timeline : Fps ou durée du film = 0";
        return;
    }
    
    QHBoxLayout* ButtonLayout = new QHBoxLayout(this);
    ButtonLayout->setContentsMargins(0, 0, 0, 0); 

    m_splitShotBtn = new ToolbarButton(this, "split_shot_white", TextManager::instance().get("tooltip_split_shot"));
    connect(m_splitShotBtn, &ToolbarButton::pressed, this, &TimelineWidget::splitCurrentShotItem);
    m_splitShotBtn->setContentsMargins(0, 0, 0, 0); 
    ButtonLayout->addWidget(m_splitShotBtn);

    m_abLoopBtn = new ToolbarButton(this, "abloop_white", TextManager::instance().get("tooltip_ab_loop"));
    connect(m_abLoopBtn, &ToolbarButton::pressed, this, &TimelineWidget::ABAction);
    m_abLoopBtn->setContentsMargins(0, 0, 0, 0); 
    ButtonLayout->addWidget(m_abLoopBtn);

    ButtonLayout->addStretch(1);
    layout->addLayout(ButtonLayout);

    m_pixelsPerMs = m_sceneWidth / static_cast<double>(m_duration);

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_sceneHeight);

    m_view = new TimelineView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop); 
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // évite que le curseur ne soit pas completement effacé quand on scroll
    connect(m_view, &TimelineView::zoomRequested, this, &TimelineWidget::applyZoom);
    connect(m_view, &TimelineView::cursorPositionRequested, this, &TimelineWidget::moveCursor);
    connect(m_view, &TimelineView::itemLeftClick, this, &TimelineWidget::itemLeftClick);
    connect(m_view, &TimelineView::itemRightClick, this, &TimelineWidget::itemRightClick);

    layout->addWidget(m_view);


    m_ruler = new RulerItem(m_sceneWidth, m_rulerHeight, m_minPxBetweenTicks, m_pixelsPerMs, m_duration, m_fps);
    m_ruler->setPos(0, 0); 
    m_scene->addItem(m_ruler);

    m_cursor = new CursorItem(m_sceneHeight);
    m_cursor->setPos(200, 0);
    m_scene->addItem(m_cursor);

    int startShotHeight = 50;

    for ( auto& IShot : projectShots ){
        double pos = timeToPosition(IShot.start);
        double width = timeToPosition(IShot.end) - pos;
        
        ShotItem* shot = new ShotItem(IShot, width, startShotHeight);

        m_scene->addItem(shot);
        shot->setPos(pos, 0);

        m_shotItems.append(shot);
    }

    connect(&SignalManager::instance(), &SignalManager::simpleToolbarUpdateCursorPosition, this, &TimelineWidget::updateCursorPos);
    connect(this, &TimelineWidget::timelineSetPosition, &SignalManager::instance(), &SignalManager::timelineSetPosition);
}


void TimelineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); 

    int viewportHeight = m_view->viewport()->height();
    int viewportWidth = m_view->viewport()->width();

    if (m_scene) {
        if(m_sceneWidth < viewportWidth){
            if(m_duration > 0.0){
                m_pixelsPerMs = viewportWidth / static_cast<double>(m_duration);
            }
            m_scene->setSceneRect(0, 0, viewportWidth, viewportHeight);
        }else {
            m_scene->setSceneRect(0, 0, m_sceneWidth, viewportHeight);
        }
        if(m_ruler){
            m_ruler->setSize(m_sceneWidth, m_rulerHeight, m_pixelsPerMs);
        }
        
        updateMarkerPos();
        updateShotItems();
        
        if(m_cursor){
            updateCursorPos(m_vlcTime);
        }
    }
}

/// @brief Reçoit le temps vlc et met à jour la position en conséquence
/// @param vlcTime 
void TimelineWidget::updateCursorPos(int64_t vlcTime){
    
    auto abData = getABLoopData();
    if(abData.has_value()){
        auto [aTime, aXPos, bTime] = abData.value();
        if( vlcTime >= bTime || vlcTime < aTime ){
            m_cursor->setPos(aXPos,m_cursor->pos().y());
            m_vlcTime = timeAtCursor();
            emit timelineSetPosition(m_vlcTime);
            updateCurrentShot();
            return;
        }
    }

    m_vlcTime = vlcTime;
    double posCursor = static_cast<double>(vlcTime) * m_pixelsPerMs; 
    m_cursor->setPos(posCursor, 0);
    updateCurrentShot();
}

/// @brief Parcours les plan pour trouver le plan courant de gauche a droite pour trouver le plan à la ms actuelle
void TimelineWidget::updateCurrentShot(){
    int shotItemCount = static_cast<int>(m_shotItems.size());

    Q_ASSERT(shotItemCount >= 1);

    int currentShotId = 0;
    int64_t distanceToClosest = m_vlcTime - m_shotItems[0]->shot().start;

    for (int IShot = 1; IShot < shotItemCount; ++IShot){
        
        int64_t currentShotStart = m_shotItems[IShot]->shot().start;

        if( m_vlcTime < currentShotStart ) break; // si le vlc time < start on est avant le plan donc on quitte la boucle

        int64_t distance = m_vlcTime - currentShotStart;
        if(distance < distanceToClosest){
            distanceToClosest = distance;
            currentShotId = IShot;
        }
    }
    ShotItem* closestShotItem =  m_shotItems[currentShotId];
    if(m_currentShotItem != closestShotItem){
        m_currentShotItem =  closestShotItem;
        emit updateShotDetailRequested( shotItemCount, currentShotId, &m_shotItems[currentShotId]->shot());
    }
} 


/// @brief Retourne une position dans la scène depuis un temps ms 
/// @param time 
/// @return 
double TimelineWidget::timeToPosition(int64_t time){
    return static_cast<double>(time) * m_pixelsPerMs; 
}

/// @brief Agrandi ou rétrécit la scène ou fonction de la molette, recalcul ensuite la position de graphics items
/// @param zoomFactor 
/// @param mouseX 
void TimelineWidget::applyZoom(double zoomFactor, int mouseX) {
    double currentTimeUnderMouse = (m_view->horizontalScrollBar()->value() + mouseX) / m_pixelsPerMs;
    double newPixelsPerMs = m_pixelsPerMs * zoomFactor;

    double minWidth = m_view->viewport()->width(); // on va limiter le dézom pour qu'au minimum la scene fait la talle du viewport 
    double maxWidth = std::numeric_limits<int>::max() - 1000000.0; // en cas de vidéo très très longue le zoom peut causer des problèmes, -1 000 000 pour de la marge au cas ou
    
    double fps = ProjectManager::instance().projet()->media->fps();
    if (fps > 0) {
        double frameMs = 1000.0 / fps;
        double totalFrames = static_cast<double>(m_duration) / frameMs;
        
        maxWidth = std::min( maxWidth, (totalFrames * m_minPxBetweenTicks)); 
        // la scene fera au maximum : nb de frames * l'espacement entre les ticks
        
        // si vidéo courte, le taille du view port peut être supérieur à maxWidth
        if (maxWidth < minWidth) maxWidth = minWidth;
    }

    // limite toujours la taille mais via le ratio et plus la taille de la scene
    double minRatio = minWidth / static_cast<double>(m_duration);
    double maxRatio = maxWidth / static_cast<double>(m_duration);

    m_pixelsPerMs = std::clamp(newPixelsPerMs, minRatio, maxRatio);

    m_sceneWidth = m_duration * m_pixelsPerMs;
    m_scene->setSceneRect(0, 0, m_sceneWidth, m_scene->height());

    m_ruler->setSize(m_sceneWidth, m_rulerHeight, m_pixelsPerMs);
    updateMarkerPos();
    updateCursorPos(m_vlcTime);

    updateShotItems();

    double newPixelPos = currentTimeUnderMouse * m_pixelsPerMs;
    m_view->horizontalScrollBar()->setValue(qRound(newPixelPos - mouseX));
}

/// @brief retourne le temps en ms de la frame le plus proche du curseur
/// @return 
int64_t TimelineWidget::timeAtCursor() {
    double cursorPos = m_cursor->pos().x();
    
    double rawMs = cursorPos / m_pixelsPerMs; 

    // snap à la frame la plus proche
    if (m_fps > 0) {
        double frameMs = 1000.0 / m_fps;
        double nearestFrameIndex = std::round(rawMs / frameMs);
        rawMs = nearestFrameIndex * frameMs; 
    }

    int64_t finalMs = static_cast<int64_t>(rawMs);
    return std::clamp(finalMs, static_cast<int64_t>(0), m_duration);
}

/// @brief déplace le curseur si l'ab loop n'est pas activé, met à jour le temps et et envoie à la toolbar le nouveau temps
/// @param cursorPosX 
void TimelineWidget::moveCursor(double cursorPosX){
    if(m_abMarkersItems.size() >= 2 ) return;

    m_cursor->setPos(cursorPosX,m_cursor->pos().y());
    m_vlcTime = timeAtCursor();
    emit timelineSetPosition(m_vlcTime);
}

/// @brief retrouve le type d'object sur lequel on a cliqué, si c'est un plan, déplace le curseur au debut du plan
/// @param item 
void TimelineWidget::itemLeftClick(QGraphicsItem * item)
{
    switch( item->type() ) {
        case SLV::TypeShotItem: 
            ShotItem* shotItem = static_cast<ShotItem*>(item);
            qDebug() << "clicked shot num : " << m_shotItems.indexOf(shotItem);
            moveCursor(timeToPosition(shotItem->shot().start));
            break;
    }
}

void TimelineWidget::itemRightClick(QPoint globalPos, QGraphicsItem * item)
{
    switch( item->type() ) {
        case SLV::TypeShotItem: 
            ShotItem* shotItem = static_cast<ShotItem*>(item);
            qDebug() << "Right click";
            showContextMenuForShot(globalPos, shotItem);
            break;
    }
}


/// @brief Raccourcis le plan courant et créer une nouveau plan avec comme début la position du curseur
void TimelineWidget::splitCurrentShotItem() {
    qDebug() << "Ms au curseur " << timeAtCursor();
    
    if (!m_currentShotItem) {
        qDebug() << "Aucun plan courant";
        return;
    }

    int index = m_shotItems.indexOf(m_currentShotItem);
    if (index == -1) {
        qDebug() << "Impossible de retrouver l'index du plan courant dans la liste des shotItems";
        return;
    }

    auto cutTime = timeAtCursor();

    if (cutTime <=  m_shotItems[index]->shot().start || cutTime >=  m_shotItems[index]->shot().end) {
        qDebug() << "Curseur n'est pas sur le plan courant";
        return;
    }

    auto oldEnd =  m_shotItems[index]->shot().end;
    m_shotItems[index]->shot().end = cutTime - 1; 

    double newWidth1 = timeToPosition(m_shotItems[index]->shot().end) - timeToPosition(m_shotItems[index]->shot().start);
    m_currentShotItem->setWidth(newWidth1);

    Shot newShotData =  Shot{ "Titre", cutTime, oldEnd};

    double pos2 = timeToPosition(newShotData.start);
    double width2 = timeToPosition(newShotData.end) - pos2;
    int startShotHeight = 50;

    ShotItem* newShotItem = new ShotItem(newShotData, width2, startShotHeight);
    newShotItem->setPos(pos2, m_currentShotItem->y());
    // on insère le plan juste apres le plan cut
    m_shotItems.insert(index + 1, newShotItem);
    m_scene->addItem(newShotItem);
}

/// @brief met à jour la position / taille des plans, pendant la mise à jour des positions, désactive la mise à jour de l'affichage
void TimelineWidget::updateShotItems(){
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    m_view->setUpdatesEnabled(false);
    double newXPos{};
    double newWidth{};
    for(int IShotItem = 0; IShotItem < m_shotItems.size(); ++IShotItem){
        newXPos = m_shotItems[IShotItem]->shot().start * m_pixelsPerMs;
        m_shotItems[IShotItem]->setPos(newXPos,0.0);
        newWidth =  (m_shotItems[IShotItem]->shot().end - m_shotItems[IShotItem]->shot().start) * m_pixelsPerMs;
        m_shotItems[IShotItem]->setWidth(newWidth);
    }
    m_view->setUpdatesEnabled(true);
    m_scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    m_scene->update();
}

void TimelineWidget::goToShot(int idShot){
    if(idShot < 0 ){
        idShot = 0;
    }else if( idShot >= m_shotItems.size()){
        idShot = m_shotItems.size()-1;
    }

    moveCursor(timeToPosition(m_shotItems[idShot]->shot().start));

}


void TimelineWidget::showContextMenuForShot(const QPoint& globalPos, ShotItem* item )
{
    QMenu menu;
    QAction *actionSplit = menu.addAction(TextManager::instance().get("timeline_split_shot_at_cursor"));
    QAction *actionAB = nullptr;
    QAction *actionExtractAB = nullptr;

    switch (m_abMarkersItems.size() )
    {
    case 0:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_0"));
        break;
    case 1:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_1"));
        break;
    case 2:
        actionAB = menu.addAction(TextManager::instance().get("timeline_ab_action_2"));
        actionExtractAB = menu.addAction(TextManager::instance().get("timeline_ab_extraxt"));
        break;
    }

    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == actionSplit) {
        splitCurrentShotItem();
    }else if (selectedAction == actionAB){
        ABAction();
    }else if (selectedAction == actionExtractAB){
        qDebug() << "Extract ab segment";
    }
}

/// @brief Ajoute un marqueur si 0 ou 1 marqueur présent. Si 1 marqueur déjà présent, garde l'ordre tel que element de 0 de m_abMarkersItems est le "A".
/// Si 2 marqueurs présents, les supprimes.
void TimelineWidget::ABAction(){

    switch (m_abMarkersItems.size())
    {
    case 2:{
        for (auto* marker : m_abMarkersItems)
        {
            m_scene->removeItem(marker);
            delete marker;
            marker = nullptr;
        }
        m_abMarkersItems.clear();
        emit enableSliderRequested();
        break;
    }
    case 1 :{
        QPointF cursorPos = m_cursor->pos();
        ABMarkerItem* newMarker = new ABMarkerItem(m_sceneHeight, timeAtCursor());
        if(newMarker->time() >= m_abMarkersItems[0]->time()){
            m_abMarkersItems.append(newMarker); 
        }else if(newMarker->time() < m_abMarkersItems[0]->time()){
            m_abMarkersItems.insert(0, newMarker);
        }
        newMarker->setPos(cursorPos);
        m_scene->addItem(newMarker);
        emit disableSliderRequested();
        break;
    }
    case 0 :{
        QPointF cursorPosDefault = m_cursor->pos();
        ABMarkerItem* newMarkerDefault = new ABMarkerItem(m_sceneHeight, timeAtCursor());
        m_abMarkersItems.append(newMarkerDefault);
        newMarkerDefault->setPos(cursorPosDefault);
        m_scene->addItem(newMarkerDefault);
        break;
    }

    }
}


std::optional<ABLoopData> TimelineWidget::getABLoopData(){
    if (m_abMarkersItems.size() < 2) return std::nullopt;

    return ABLoopData{m_abMarkersItems[0]->time(), m_abMarkersItems[0]->pos().x(), m_abMarkersItems[1]->time()};

}

void TimelineWidget::updateMarkerPos(){
    double newXPos{};
    for( auto* marker : m_abMarkersItems){
        newXPos = marker->time() * m_pixelsPerMs;
        marker->setX(newXPos);
    }
}
    
