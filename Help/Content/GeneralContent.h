#ifndef GENERALCONTENT_H
#define GENERALCONTENT_H

#include "../Base/CategoryBase.h"

class GeneralContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit GeneralContent(QWidget* parent = nullptr);

private:
    QWidget* introduction(const QString& subcategoryName);
    QWidget* reportIssue(const QString& subcategoryName);
};

#endif