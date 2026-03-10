#ifndef SHOTDETAIL_H
#define SHOTDETAIL_H

#include "Shot.h"

#include <QWidget>
#include <QLabel>

class ShotDetail : public QWidget
{
Q_OBJECT

public:
    explicit ShotDetail(QWidget* parent = nullptr);

public slots:
    void updateShotDetail(Shot*);

private:
    QLabel* m_titleLabel = nullptr;
    QLabel* m_startLabel = nullptr;
    QLabel* m_endLabel = nullptr;
};





#endif // SHOTDETAIL_H