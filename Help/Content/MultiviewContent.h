#ifndef MULTIVIEWCONTENT_H
#define MULTIVIEWCONTENT_H

#include "../Base/CategoryBase.h"

class MultiviewContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit MultiviewContent(QWidget* parent = nullptr);

    private:
    QWidget *presentation(const QString &subcategoryName);
    QWidget* openMultiviewMode(const QString& subcategoryName);
    QWidget *duplicateFile(const QString &subcategoryName);
    QWidget *manageMultiview(const QString &subcategoryName);

};

#endif