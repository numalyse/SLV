#ifndef TIMELINERULER_H
#define TIMELINERULER_H

#include <QWidget>

class TimelineRuler : public QWidget
{
Q_OBJECT

public:
    explicit TimelineRuler(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

};


#endif // TIMELINERULER_H