#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include <QDialog>

class AboutWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWidget(QWidget *parent = nullptr);
    ~AboutWidget();
};

#endif // ABOUTWIDGET_H