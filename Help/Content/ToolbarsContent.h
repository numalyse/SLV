#ifndef TOOLBARSCONTENT_H
#define TOOLBARSCONTENT_H

#include "../Base/CategoryBase.h"

class ToolbarsContent : public CategoryBase
{
    Q_OBJECT

public:
    explicit ToolbarsContent(QWidget* parent = nullptr);

private:
    QWidget* introduction();
    QWidget* player();
    QWidget *classicToolbar(const QString& subcategoryName);
    QWidget *extendedToolbar(const QString& subcategoryName);
    QWidget *timelineToolbar(const QString& subcategoryName);
};

#endif