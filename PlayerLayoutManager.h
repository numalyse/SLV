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

    void createLayout(const int count, const PlayerLayoutArrangement& arrangement = PlayerLayoutArrangement::ArrangementUnknown);

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
    QWidget* create2(const QStringList& filesPaths = QStringList(""), const Qt::Orientation& orientation = Qt::Horizontal);
    QWidget* create3(const QStringList& filesPaths = QStringList(""), const PlayerLayoutArrangement& arrangement = PlayerLayoutArrangement::ArrangementUnknown);
    QWidget* create4(const QStringList& filesPaths = QStringList(""));

    /// @brief Créer une toolbar globale et la connecte aux players
    Toolbar* createGlobalToolbar();
    /// @brief Créer une toolbar avancée et la connecte au player
    Toolbar* createAdvancedToolbar();
    /// @brief Créer la bonne toolbar en fonction du nombre de players actifs
    Toolbar* createLayoutToolbar();

    bool newGlobalMuteState();
    bool newGlobalPlayState();


signals:
    void updateContainerRequest(Media*, QWidget*, Toolbar*);

    void enableFullscreenPlayerRequested();
    void disableFullscreenPlayerRequested();

    void enableFullscreenGlobalRequested();
    void disableFullscreenGlobalRequested();
    void enableFullscreenAdvancedRequested();
    void disableFullscreenAdvancedRequested();

    void setGlobalPlayStateRequested(bool);
    void setGlobalMuteStateRequested(bool);

    void disableNavPanelRequested();
    void enableNavPanelRequested();

    void previousMediaRequested();
    void nextMediaRequested();

    void buttonsDisabled();

public slots:
    void duplicatePlayer(PlayerWidget* toBeDuplicated);

    void removePlayer(PlayerWidget* playerToDestroy);

    void enablePlayerLayoutFullscreen(PlayerWidget* playerToFullscreen);
    void disablePlayerLayoutFullscreen(PlayerWidget* playerToFullscreen);

    void enableGlobalLayoutFullscreen();
    void disableGlobalLayoutFullscreen();

    void checkPlayersPlayStatus();
    void checkPlayersMuteStatus();

    void createLayoutFromPaths(const QStringList& filesPaths);

    // void enableButtons();
    void disableGlobalToolbarButtons();
    void arrangePlayerLayout(const PlayerLayoutArrangement&);
};

#endif // PLAYERLAYOUTMANAGER_H
