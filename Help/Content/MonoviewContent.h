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
    QWidget *openMedia(const QString &subcategoryName);
    QWidget *controlMedia(const QString& subcategoryName);
    QWidget *paramMedia(const QString& subcategoryName);
    QWidget *zoomMedia(const QString& subcategoryName);
    QWidget *altMedia(const QString& subcategoryName);
    QWidget *drawingMode(const QString& subcategoryName);
    QWidget *compoRules(const QString& subcategoryName);
    QWidget *screenshot(const QString& subcategoryName);
    QWidget *recording(const QString& subcategoryName);
    QWidget *extractSequence(const QString &subcategoryName);
};

#endif