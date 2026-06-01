#ifndef HELPERVIDGET_H
#define HELPERVIDGET_H

#include <QDialog>

class HelperWidget : public QDialog
{
    Q_OBJECT

public:
    explicit HelperWidget(QWidget *parent = nullptr);
    ~HelperWidget();
};

#endif // HELPERVIDGET_H