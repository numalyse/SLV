#ifndef MONOVIEWCONTENT_H
#define MONOVIEWCONTENT_H

#include "../Base/CategoryBase.h"

class MonoviewContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit MonoviewContent(QWidget* parent = nullptr);

private:
    QWidget* introduction();
    QWidget* player();
};

#endif