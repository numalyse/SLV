#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include <QWidget>
#include "PlayerLayoutManager.h"

class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);
    QWidget* m_currentLayout = nullptr;

private:
    PlayerLayoutManager* m_layoutManager = nullptr;

    QVBoxLayout* layout;
public slots:
    void setPlayers();
    void setPlayersFromPaths(QStringList);

signals:
};

#endif // GLOBALPLAYERMANAGER_H
