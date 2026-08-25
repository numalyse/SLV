#ifndef MONOVIEWCONTENT_H
#define MONOVIEWCONTENT_H

#include "../Base/CategoryBase.h"

class MonoviewContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit MonoviewContent(QWidget* parent = nullptr);

private:
    QWidget *presentation(const QString &subcategoryName);
    QWidget *openmedia(const QString &subcategoryName);
    QWidget* player(const QString& subcategoryName);
};

#endif