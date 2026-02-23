#ifndef PLAYERLAYOUTMANAGER_H
#define PLAYERLAYOUTMANAGER_H

#include <QObject>

class PlayerWidget;

class PlayerLayoutManager : public QObject
{
    Q_OBJECT
public:
    explicit PlayerLayoutManager(QObject *parent = nullptr);

    QWidget* createLayout(const QVector<PlayerWidget*> &players, int count);

private:
    QWidget* create1(const QVector<PlayerWidget*>& players);
    QWidget* create2(const QVector<PlayerWidget*>& players);
    QWidget* create3(const QVector<PlayerWidget*>& players);
    QWidget* create4(const QVector<PlayerWidget*>& players);

signals:
};

#endif // PLAYERLAYOUTMANAGER_H
