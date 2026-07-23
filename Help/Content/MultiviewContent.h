#ifndef MULTIVIEWCONTENT_H
#define MULTIVIEWCONTENT_H

#include "../Base/CategoryBase.h"

class MultiviewContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit MultiviewContent(QWidget* parent = nullptr);

private:
    QWidget* introduction(const QString& subcategoryName);
};

#endif