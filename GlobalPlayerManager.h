#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include <QWidget>
#include "PlayerLayoutManager.h"

class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);
private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QWidget* m_currentLayout = nullptr;
public slots:
    void setPlayers();

signals:
};

#endif // GLOBALPLAYERMANAGER_H
