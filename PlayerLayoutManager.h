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

    QWidget* createLayout(const int count);
    QWidget* createLayoutFromPaths(const QStringList& filesPaths);

private:
    QWidget* create1(const QStringList& filePath = QStringList(""));
    QWidget* create2(const QStringList& filesPaths = QStringList(""));
    QWidget* create3(const QStringList& filesPaths = QStringList(""));
    QWidget* create4(const QStringList& filesPaths = QStringList(""));

signals:
    void updateContainer(QWidget*);

public slots:
    void addPlayer2();
    void removePlayer2(PlayerWidget* playerToDestroy);

};

#endif // PLAYERLAYOUTMANAGER_H
