#ifndef TIMELINE_H
#define TIMELINE_H

#include "RulerItem.h"
#include "CursorItem.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsScene>
#include <QVBoxLayout>

class TimelineWidget : public QWidget
{

public:
    explicit TimelineWidget(QWidget* parent = nullptr);

public slots:
    void updateCursorPos(int64_t vlcTime);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QGraphicsScene* m_scene = nullptr;
    QGraphicsView* m_view = nullptr;
    QVBoxLayout* m_layout = nullptr;

    RulerItem* m_ruler = nullptr;
    CursorItem* m_cursor = nullptr;

    int m_cursorPosition = 250;
    int m_sceneWidth = 5000;
    int m_sceneHeight = 150;
    int m_rulerHeight = 30;
    double m_currentScale = 1;
};



#endif // TIMELINE_H
