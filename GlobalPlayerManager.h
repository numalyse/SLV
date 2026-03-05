#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include "Toolbars/Toolbar.h"
#include "PlayerLayoutManager.h"
#include "NavPanel.h"

#include <QWidget>


class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);

private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QWidget* m_playersWidget = nullptr;
    Toolbar* m_toolbarWidget = nullptr;
    NavPanel* m_navPanel = nullptr;
    QVBoxLayout* layout;

public slots:


    void setPlayersFromPaths(QStringList);
    void enableFullscreenGlobal();
    void disableFullscreenGlobal();

    void updateContainer(int, QWidget*, Toolbar*);
    void openNavPanel();
    void closeNavPanel();

    void setGlobalPlayState(bool);

signals:
    void enableFullscreenMainRequested();
    void disableFullscreenMainRequested();
};

#endif // GLOBALPLAYERMANAGER_H
