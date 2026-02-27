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

    void createLayout(const int count);
    void createLayoutFromPaths(const QStringList& filesPaths);

private:

    // Les 4 players sont créer à l'initialisation de PlayerLayoutManager
    // un peu plus lourd mais pas besoin de recréer des vlc instances / toolbar ect. à chaque ajout de lecteur
    QVector<PlayerWidget*> m_players;

    // Vecteur qui contient les addresses des players actuellement affichés.
    // Permet de garder une cohérence d'affichage lors de l'ajout / suppression de player
    QVector<PlayerWidget*> m_activePlayers;

    static constexpr int s_maxPlayerCount = 4;

    /// @brief Modifie le parent des playerWidgets
    /// Evite qu'ils soient supprimé lorsque GlobalPlayerManager appelle deleteLater()
    void detachAllPlayers();

    /// @brief Ajoute ou Supprime des playerWidgets dans m_activePlayers 
    /// @param activePlayersNeeded Le nombre de playerWidgets que doit avoir le nouveau widget
    void activePlayerUpdate(const int activePlayersNeeded);

    QWidget* create1(const QStringList& filePath = QStringList(""));
    QWidget* create2(const QStringList& filesPaths = QStringList(""));
    QWidget* create3(const QStringList& filesPaths = QStringList(""));
    QWidget* create4(const QStringList& filesPaths = QStringList(""));

    /// @brief Créer une toolbar globale et la connecte aux players
    Toolbar* createGlobalToolbar();
    /// @brief Créer une toolbar avancée et la connecte au player
    Toolbar* createAdvancedToolbar();
    /// @brief Créer la bonne toolbar en fonction du nombre de players actifs
    Toolbar* createLayoutToolbar();

signals:
    /// @brief Envoie au globalPlayerManager le nouveau playersWidget et une toolbar connecté aux players
    void updateContainerRequest(int, QWidget*, Toolbar*);

public slots:
    /// @brief Créer un nouveau widget à envoyer
    /// Le widget aura m_activePlayers.size()+1 PlayersWidgets
    void addPlayer();

    /// @brief Créer un nouveau widget à envoyer 
    /// Le widget aura m_activePlayers.size()-1 PlayersWidgets
    /// @param playerToDestroy Le playerWidget à supprimer de l'affichage
    void removePlayer(PlayerWidget* playerToDestroy);

};

#endif // PLAYERLAYOUTMANAGER_H
