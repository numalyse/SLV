#ifndef NAVPANEL_H
#define NAVPANEL_H

#include <QWidget>
#include <QBoxLayout>

class NavPanel : public QWidget
{
    Q_OBJECT
public:
    explicit NavPanel(QWidget *parent = nullptr);
private:

    bool m_isOpen = false;
    QWidget *m_sideWidget = nullptr;

public slots:
    void showPanel();
    void hidePanel();

signals:
};

#endif // NAVPANEL_H
