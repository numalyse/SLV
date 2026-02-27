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

    QWidget* createLayout(const int count);
    QWidget* createLayoutFromPaths(const QStringList& filesPaths);

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

signals:
    void updateContainerRequest(int, QWidget*);

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
