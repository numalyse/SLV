#ifndef GLOBALPLAYERMANAGER_H
#define GLOBALPLAYERMANAGER_H

#include "Toolbars/Toolbar.h"

#include <QWidget>
#include "PlayerLayoutManager.h"

class GlobalPlayerManager : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPlayerManager(QWidget *parent = nullptr);
    QWidget* m_playersWidget = nullptr;
    Toolbar* m_toolbarWidget = nullptr;

private:
    PlayerLayoutManager* m_layoutManager = nullptr;
    QVBoxLayout* layout;

public slots:

    ///@brief Supprime puis Remplace m_playersWidget par le widget retourné par m_layoutManager->createLayoutFromPaths(filesPaths); 
    void setPlayersFromPaths(QStringList);

    /// @brief Supprime le QWidget m_playersWidget pour libérer la mémoire et met à jour m_playersWidget avec le Widget envoyé par PlayerLayoutManager
    /// @param Le nombre de players dans le nouveau widget, permet de mettre à jour l'ui en fonction du nombre
    /// @param Le Widget qui va remplacer m_playersWidget
    void updateContainer(int, QWidget*);

signals:
};

#endif // GLOBALPLAYERMANAGER_H
