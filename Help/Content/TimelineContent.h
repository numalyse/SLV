#ifndef TIMELINECONTENT_H
#define TIMELINECONTENT_H

#include "../Base/CategoryBase.h"

class TimelineContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit TimelineContent(QWidget* parent = nullptr);

    
    private:
    QWidget *presentation(const QString &subcategoryName);
    QWidget* introduction(const QString& subcategoryName);
};

#endif