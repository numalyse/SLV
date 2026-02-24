#ifndef PLAYERLAYOUTMANAGER_H
#define PLAYERLAYOUTMANAGER_H

#include <QObject>

#include "PlayerWidget.h"

class PlayerWidget;

class PlayerLayoutManager : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerLayoutManager(QObject *parent = nullptr);
    ~PlayerLayoutManager();

    QVector<PlayerWidget*> m_players;

    QWidget* createLayout();

private:
    QWidget* create1();
    QWidget* create2();
    QWidget* create3();
    QWidget* create4();

signals:
    void updateContainer(QWidget*);

public slots:
    void addPlayer2();
    void removePlayer2(PlayerWidget* playerToDestroy);

};

#endif // PLAYERLAYOUTMANAGER_H
